#include "ns3/internet-module.h"
#include <stdlib.h>
#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"
#include "ns3/address-utils.h"
#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/event-id.h"
#include "ouroboros-node.h"
#include "constants.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("OuroborosNodeApp");
    NS_OBJECT_ENSURE_REGISTERED (OuroborosNodeApp);

    OuroborosNodeApp::OuroborosNodeApp( OuroborosHelper *nodeHelper): BlockChainNodeApp(nodeHelper) {
        this->nodeHelper = nodeHelper;
        this->IamLeaderCounter = 0;
    }

    void OuroborosNodeApp::StartApplication() {
        NS_LOG_FUNCTION(this);
        NS_LOG_INFO("Starting Ouroboros App " << GetNode()->GetId());
        BlockChainNodeApp::StartApplication();
        this->newSlotNextEvent = Simulator::Schedule(Seconds(this->nodeHelper->GetSlotSizeSeconds()), &OuroborosNodeApp::StartNewSlot, this);
        this->sendingSeedNextEvent = Simulator::Schedule(Seconds(0.0), &OuroborosNodeApp::SendEpochSeed, this);
    }

    void OuroborosNodeApp::StopApplication() {
        NS_LOG_FUNCTION(this);
        BlockChainNodeApp::StopApplication();
        Simulator::Cancel(this->sendingSeedNextEvent);
        Simulator::Cancel(this->newSlotNextEvent);
    }

    bool OuroborosNodeApp::HandleCustomRead(Ptr <Packet> packet, Address from, std::string receivedData, rapidjson::Document *document) {
//        NS_LOG_INFO("Node " << GetNode()->GetId() << " Total Received Data: " << receivedData);

        if (!document->IsObject()) {
            NS_LOG_WARN("The parsed packet is corrupted: " << receivedData);
            return false;
        }
        int messageType = (*document)["type"].GetInt();
        switch (messageType) {
            case OUROBOROS_SEED:
                this->ReceiveEpochSeed(document);
                return true;
        }
        return false;
    }

    void OuroborosNodeApp::FinishActualSlot() {
        NS_LOG_FUNCTION(this);

        Block* lastBlock = this->blockChain->GetTopBlock();
        int blockHeight =  this->blockChain->GetBlockchainHeight()+1;
        int validator = GetNode()->GetId();
        double timeSeconds = Simulator::Now().GetSeconds();
        int slotNum = this->GetSlotNumber() - 1;

        this->SortReceivedTransactionsByFee();
        Block *newBlock = new Block(blockHeight, validator, lastBlock, timeSeconds, timeSeconds, Ipv4Address("0.0.0.0"));
        newBlock->SetLoopNumber(slotNum);
        int transactionsInBlockCounter = 0;
        for (auto trans: this->receivedTransactions) {
            newBlock->AddTransaction(trans);
            transactionsInBlockCounter++;
        }
        if(this->IsIamLeader(slotNum)){
            this->IamLeaderCounter++;
            this->blockChain->AddBlock(newBlock);
            rapidjson::Document transactionDoc = newBlock->ToJSON();
            this->SendMessage(&transactionDoc, this->broadcastSocket);
        }
        this->receivedTransactions.erase(this->receivedTransactions.begin(),this->receivedTransactions.begin()+transactionsInBlockCounter);
    }

    void OuroborosNodeApp::StartNewSlot() {
        this->FinishActualSlot();

        //plan next slot event
        this->newSlotNextEvent = Simulator::Schedule(Seconds(this->nodeHelper->GetSlotSizeSeconds()), &OuroborosNodeApp::StartNewSlot, this);
    }


    void OuroborosNodeApp::ReceiveNewTransaction(rapidjson::Document *message){
        NS_LOG_FUNCTION(this);
//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " receive new Transaction");
        BlockChainNodeApp::ReceiveNewTransaction(message);
    }

    int OuroborosNodeApp::GetSlotLeader(int slotNumber, int epochNumber){
        if((int)this->receivedSeeds.size() <= epochNumber){
            NS_LOG_ERROR("Can not generate slot leader - epoch:" << epochNumber << " slot: " << slotNumber << " - I did not receive any seed" );
            return -1;
        }
        if(this->receivedSeeds[epochNumber].size() < constants.numberOfNodes){
            NS_LOG_ERROR("Can not generate slot leader - epoch:" << epochNumber << " slot: " << slotNumber << " - I did not receive all seed" );
            return -1;
        }
        return this->nodeHelper->GetSlotLeader(slotNumber);
    }

    bool OuroborosNodeApp::IsIamLeader() {
        int leaderId = this->nodeHelper->GetSlotLeader(this->GetSlotNumber());
        if((int)GetNode()->GetId() == leaderId){
            return true;
        } else {
            return false;
        }
    }

    bool OuroborosNodeApp::IsIamLeader(int slotNumber) {
        int leaderId = this->nodeHelper->GetSlotLeader(slotNumber);
        if((int)GetNode()->GetId() == leaderId){
            return true;
        } else {
            return false;
        }
    }

    void OuroborosNodeApp::SendEpochSeed() {
        NS_LOG_FUNCTION(this);

        int secret = this->CreateSecret();
        int epochNum = this->GetEpochNumber() + 1;  //for future epoch
//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << "s NODE " << GetNode()->GetId() << " Epoch:  " << epochNum << " sending seed: " << secret);

        this->SaveEpochNum(epochNum,secret,GetNode()->GetId());

        const char *json = "{\"type\":\"1\",\"value\":\"1\", \"epochNum\":\"1\",\"senderId\":\"1\"}";
        rapidjson::Document message;
        message.Parse(json);
        message["type"].SetInt(OUROBOROS_SEED);
        message["value"].SetInt(secret);
        message["epochNum"].SetInt(epochNum);
        message["senderId"].SetInt(GetNode()->GetId());

        this->SendMessage(&message, this->broadcastSocket);

        //plan next sending
        this->sendingSeedNextEvent = Simulator::Schedule(Seconds(this->nodeHelper->GetEpochSizeSeconds()), &OuroborosNodeApp::SendEpochSeed, this);
    }

    void OuroborosNodeApp::ReceiveEpochSeed(rapidjson::Document *message) {
        int seed = (*message)["value"].GetInt();
        int epochNum = (*message)["epochNum"].GetInt();
        int nodeId = (*message)["senderId"].GetInt();

        //save received epoch num
        bool saved = this->SaveEpochNum(epochNum,seed,nodeId);

        //resend epoch number
        if(saved){
            this->SendMessage(message, this->broadcastSocket);
        }

    }

    bool OuroborosNodeApp::SaveEpochNum(int epochNum, int value, int nodeId){
        //return true -> save, false -> not saved (maybe already exist)
        if(epochNum >= (int)this->receivedSeeds.size()){
            this->receivedSeeds.resize(epochNum+1);
            std::vector<int> epochRecSeeds;
            epochRecSeeds.resize(constants.numberOfNodes,0);
            this->receivedSeeds[epochNum] = epochRecSeeds;
        }

        if(this->receivedSeeds[epochNum][nodeId] != 0) {
            return false;
        }
        this->receivedSeeds[epochNum][nodeId] = value;
        return true;
    }

    int OuroborosNodeApp::CreateSecret() {
        return rand();
    }

    int OuroborosNodeApp::GetSlotNumber() {
        return this->nodeHelper->GetSlotNumber();
    }

    int OuroborosNodeApp::GetEpochNumber() {
        return this->nodeHelper->GetEpochNumber();
    }

    void OuroborosNodeApp::PrintProcessInfo() {
        int seedCounter = 0;
        for(auto item: this->receivedSeeds) {
            seedCounter = item.size();
        }

        NS_LOG_FUNCTION(this);
        NS_LOG_INFO("");
        NS_LOG_INFO(" Ouroboros App " << GetNode()->GetId());
        NS_LOG_INFO("----------------------------------------------------------------------------------------   ");
        NS_LOG_INFO(" Ouroboros Info: ");
        NS_LOG_INFO(" Epoch count   |  SlotCount count  |   Received seeds  |");
        NS_LOG_INFO("       " << this->nodeHelper->GetEpochNumber()+1 << "       |         " << this->nodeHelper->GetSlotNumber()+1 << "        |        " << seedCounter << "         |");
        NS_LOG_INFO(" I am leader count   | ");
        NS_LOG_INFO("         " << this->IamLeaderCounter << "         | ");
        NS_LOG_INFO(" Base Info: ");
        BlockChainNodeApp::PrintProcessInfo();
        NS_LOG_INFO("");
    }
}