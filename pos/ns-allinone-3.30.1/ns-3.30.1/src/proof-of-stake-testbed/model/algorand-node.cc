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
#include "algorand-node.h"
#include "constants.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("AlgorandNodeApp");
    NS_OBJECT_ENSURE_REGISTERED (AlgorandNodeApp);

    AlgorandNodeApp::AlgorandNodeApp (AlgorandHelper *nodeHelper) : BlockChainNodeApp(nodeHelper) {
        this->nodeHelper = nodeHelper;
        this->loopCounterProposedBlock = 0;
        this->loopCounterSoftVote = 0;
        this->loopCounterCertifyVote = 0;
        this->secondsWaitingForBlockReceive = constants.algorandWaitingProposedBlockSeconds;
        this->secondsWaitingForStartSoftVote = 1.0;
        this->secondsWaitingForStartCertify = 2.5;

        //commite size Generator
        std::poisson_distribution<int> d1(constants.algorandPoissonDistributionMeanCount);
        this->committeeSizeGenerationDistribution = d1;

        this->timeWaitingBlock.push_back( time( 0 ) );
    }

    void AlgorandNodeApp::StartApplication() {
        NS_LOG_FUNCTION(this);
        NS_LOG_INFO("Starting Algorand App " << GetNode()->GetId());
        BlockChainNodeApp::StartApplication();

        //start events
        this->blockProposeEvent = Simulator::Schedule(Seconds(this->secondsWaitingForBlockReceive), &AlgorandNodeApp::FinishReceiveTransaction, this);
        this->SoftVoteEvent = Simulator::Schedule(Seconds(this->secondsWaitingForBlockReceive+this->secondsWaitingForStartSoftVote),
                                                  &AlgorandNodeApp::SoftVotePhase, this);
        this->CertifyVoteEvent = Simulator::Schedule(Seconds(this->secondsWaitingForBlockReceive+this->secondsWaitingForStartCertify),
                                                  &AlgorandNodeApp::CertifyVotePhase, this);
    }

    void AlgorandNodeApp::StopApplication() {
        NS_LOG_FUNCTION(this);
        BlockChainNodeApp::StopApplication();
        Simulator::Cancel(this->blockProposeEvent);
        Simulator::Cancel(this->SoftVoteEvent);
        Simulator::Cancel(this->CertifyVoteEvent);
    }

    bool AlgorandNodeApp::IsIBlockProposalMember() {
        auto listOfMembers = this->nodeHelper->ListOfBlockProposals(this->loopCounterProposedBlock);
        for(int nodeId: listOfMembers) {
            if(nodeId == (int)GetNode()->GetId()){
                return true;
            }
        }
        return false;
    }

    bool AlgorandNodeApp::IsISoftCommitteeMember() {
        auto listOfMembers = this->nodeHelper->ListOfCommitteeMembers(this->loopCounterSoftVote);
        for(int nodeId: listOfMembers) {
            if(nodeId == (int)GetNode()->GetId()){
                return true;
            }
        }
        return false;
    }

    bool AlgorandNodeApp::IsICertifyCommitteeMember() {
        auto listOfMembers = this->nodeHelper->ListOfCommitteeMembers(this->loopCounterCertifyVote);
        for(int nodeId: listOfMembers) {
            if(nodeId == (int)GetNode()->GetId()){
                return true;
            }
        }
        return false;
    }

    bool AlgorandNodeApp::HandleCustomRead(Ptr <Packet> packet, Address from, std::string receivedData, rapidjson::Document *document) {
        NS_LOG_FUNCTION(this);
//        NS_LOG_INFO("Node " << GetNode()->GetId() << " time " << Simulator::Now().GetSeconds() << " Total Received Data: " << receivedData);

        if (!document->IsObject()) {
            NS_LOG_WARN("The parsed packet is corrupted: " << receivedData);
            return false;
        }
        int messageType = (*document)["type"].GetInt();
        switch (messageType) {
            case ALGORAND_BLOCK_PROPOSAL:
                this->ReceiveProposedBlock(document);
                return true;
            case ALGORAND_SOFT_VOTE:
                this->ReceiveSoftVote(document);
                return true;
            case ALGORAND_CERTIFY_VOTE:
                this->ReceiveCertifyVote(document);
                return true;
        }
        return false;
    }

    void AlgorandNodeApp::ReceiveProposedBlock(rapidjson::Document *message) {
        this->timeWaitingBlock.push_back( time( 0 ) );
        NS_LOG_FUNCTION(this);

        Block *previousBlock = this->blockChain->GetTopBlock();
        Block *proposedBlock = Block::FromJSON(message,previousBlock,Ipv4Address("0.0.0.0"));

        //allocate new size in vector
        if(proposedBlock->GetLoopNumber() >= (int)this->receivedProposedBlocks.size()){
            int lastSize = (int)this->receivedProposedBlocks.size();
            this->receivedProposedBlocks.resize(proposedBlock->GetLoopNumber()+1);
            for(int i=lastSize; i <= proposedBlock->GetLoopNumber(); i++){
                std::vector <Block *> vector;
                this->receivedProposedBlocks[i] = vector;
            }
        }

        //check if node has not already received this proposed block
        previousBlock = this->blockChain->GetTopBlock();
        proposedBlock = Block::FromJSON(message,previousBlock,Ipv4Address("0.0.0.0"));
        for(auto block: this->receivedProposedBlocks[proposedBlock->GetLoopNumber()]){
            if(proposedBlock->IsSameAs(block)){
                //already receive
                return;
            }
        }

        //section where first time proposed block come
        this->receivedProposedBlocks[proposedBlock->GetLoopNumber()].push_back(proposedBlock);
        this->SendMessage(message, this->broadcastSocket);
    }

    Block* AlgorandNodeApp::GetLowerReceivedProposedBlock(int loopCounter) {
        Block *selectedBlock;
        long int lowerNum;

        if((int)this->receivedProposedBlocks.size() <= loopCounter){
            return NULL;
        }
        if((int)this->receivedProposedBlocks[loopCounter].size() == 0){
            return NULL;
        }

        selectedBlock = this->receivedProposedBlocks[loopCounter][0];
        lowerNum = selectedBlock->GetId();
        for(auto block: this->receivedProposedBlocks[loopCounter]){
            if(block->GetId() < lowerNum){
                selectedBlock = block;
                lowerNum = selectedBlock->GetId();
            }
        }
        return selectedBlock;
    }

    void AlgorandNodeApp::ReceiveNewTransaction(rapidjson::Document *message){
        NS_LOG_FUNCTION(this);
//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " receive new Transaction");
        BlockChainNodeApp::ReceiveNewTransaction(message);
    }

    void AlgorandNodeApp::FinishReceiveTransaction() {
        NS_LOG_FUNCTION(this);
        double timeSeconds = Simulator::Now().GetSeconds();
//        NS_LOG_INFO("At time " << timeSeconds << " node " << GetNode()->GetId() << " loop " << this->loopCounterProposedBlock << " end with receive new transaction ");

        //create new block
        Block* lastBlock = this->blockChain->GetTopBlock();
        int blockHeight =  this->blockChain->GetBlockchainHeight()+1;
        int validator = GetNode()->GetId();
        timeSeconds = Simulator::Now().GetSeconds();

        //send created block
        this->SortReceivedTransactionsByFee();
        Block *newBlock = new Block(blockHeight, validator, lastBlock, timeSeconds, timeSeconds, Ipv4Address("0.0.0.0"));
        newBlock->SetLoopNumber(this->loopCounterProposedBlock);
//            NS_LOG_INFO("At time " << timeSeconds << " node " << GetNode()->GetId() << " loop " << this->loopCounterProposedBlock << " propose block: " << newBlock->GetId());
        int transactionsInBlockCounter = 0;
        for (auto trans: this->receivedTransactions) {
            newBlock->AddTransaction(trans);
            transactionsInBlockCounter++;
        }
        if(this->IsIBlockProposalMember()) {
            rapidjson::Document transactionDoc = newBlock->ToJSON();
            transactionDoc["type"] = ALGORAND_BLOCK_PROPOSAL;
            this->SendMessage(&transactionDoc, this->broadcastSocket);
        }
        this->receivedTransactions.erase(this->receivedTransactions.begin(),this->receivedTransactions.begin()+transactionsInBlockCounter);

        //plan next events
        this->loopCounterProposedBlock++;
        this->blockProposeEvent = Simulator::Schedule(Seconds(this->secondsWaitingForBlockReceive), &AlgorandNodeApp::FinishReceiveTransaction, this);
    }


    void AlgorandNodeApp::SoftVotePhase() {
        NS_LOG_FUNCTION(this);
        if(!this->IsISoftCommitteeMember()){
            this->loopCounterSoftVote++;
            this->SoftVoteEvent = Simulator::Schedule(Seconds(this->secondsWaitingForBlockReceive), &AlgorandNodeApp::SoftVotePhase, this);
            return;
        }
//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " loop " << this->loopCounterSoftVote << " start soft vote phase");

        Block *block = this->GetLowerReceivedProposedBlock(this->loopCounterSoftVote);
        if(block == NULL){
            return;
        }

        int nodeId = GetNode()->GetId();
//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " start soft vote phase with " << block->GetId());
        const char *json = "{\"type\":\"1\",\"blockId\":\"1\", \"loopNum\":\"1\",\"senderId\":\"1\", \"senderStack\":\"1\"}";
        rapidjson::Document message;
        message.Parse(json);
        message["type"].SetInt(ALGORAND_SOFT_VOTE);
        message["blockId"].SetInt(block->GetId());
        message["loopNum"].SetInt(block->GetLoopNumber());
        message["senderId"].SetInt(GetNode()->GetId());
        message["senderStack"].SetInt(this->nodeHelper->GetNodeStack(nodeId));

        this->SendMessage(&message, this->broadcastSocket);

        //plan next
        this->loopCounterSoftVote++;
        this->SoftVoteEvent = Simulator::Schedule(Seconds(this->secondsWaitingForBlockReceive), &AlgorandNodeApp::SoftVotePhase, this);
    }

    void AlgorandNodeApp::ReceiveSoftVote(rapidjson::Document *message) {
        NS_LOG_FUNCTION(this);

        int recBlockId = (*message)["blockId"].GetInt();
        int loopOfBlock = (*message)["loopNum"].GetInt();

//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " loop " << loopOfBlock << " receive soft vote");

        if(loopOfBlock >= (int)this->receivedSoftVoteBlockIds.size()){
            int lastSize = (int)this->receivedSoftVoteBlockIds.size();
            this->receivedSoftVoteBlockIds.resize(loopOfBlock+1);
            for(int i=lastSize; i <= loopOfBlock; i++){
                std::vector <int> vector;
                this->receivedSoftVoteBlockIds[i] = vector;
            }
        }

        //check if node has not already received this soft vote
        for(auto blockId: this->receivedSoftVoteBlockIds[loopOfBlock]){
            if(blockId == recBlockId){
                //already receive
                return;
            }
        }

        this->receivedSoftVoteBlockIds[loopOfBlock].push_back(recBlockId);
        this->SendMessage(message, this->broadcastSocket);
    }

    void AlgorandNodeApp::CertifyVotePhase() {
        NS_LOG_FUNCTION(this);
        if(!this->IsICertifyCommitteeMember()){
            this->loopCounterCertifyVote++;
            this->CertifyVoteEvent = Simulator::Schedule(Seconds(this->secondsWaitingForBlockReceive), &AlgorandNodeApp::CertifyVotePhase, this);
            return;
        }
//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " loop " << this->loopCounterCertifyVote << " start certify vote phase");

        Block *block = this->GetLowerReceivedProposedBlock(this->loopCounterCertifyVote);
        if(block == NULL){
            return;
        }
        int blockId = block->GetId();
        int nodeId = GetNode()->GetId();
//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " loop " << this->loopCounterCertifyVote << " start certify vote phase with " << blockId);

        //create messasge
        rapidjson::Document message;
        message.SetObject();
        int messageId = rand();
        message.AddMember("type", ALGORAND_CERTIFY_VOTE, message.GetAllocator());
        message.AddMember("messageId", messageId, message.GetAllocator());
        message.AddMember("blockId", blockId, message.GetAllocator());
        message.AddMember("loopNum", this->loopCounterCertifyVote, message.GetAllocator());
        message.AddMember("senderId", GetNode()->GetId(), message.GetAllocator());
        message.AddMember("senderStack", this->nodeHelper->GetNodeStack(nodeId), message.GetAllocator());
        message.AddMember("block", block->ToJSON(), message.GetAllocator());

        this->SendMessage(&message, this->broadcastSocket);

        //plan next
        this->loopCounterCertifyVote++;
        this->CertifyVoteEvent = Simulator::Schedule(Seconds(this->secondsWaitingForBlockReceive), &AlgorandNodeApp::CertifyVotePhase, this);
    }

    void AlgorandNodeApp::ReceiveCertifyVote(rapidjson::Document *message) {
        NS_LOG_FUNCTION(this);

        int loopOfBlock = (*message)["loopNum"].GetInt();

//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " loop " << loopOfBlock <<  " receive certify vote");

        if(loopOfBlock >= (int)this->receivedCertifyMessageIds.size()){
            int lastSize = (int)this->receivedCertifyMessageIds.size();
            this->receivedCertifyMessageIds.resize(loopOfBlock+1);
            this->receivedCertifyVoteBlockIds.resize(loopOfBlock+1);
            for(int i=lastSize; i <= loopOfBlock; i++){
                std::vector <int> vector;
                this->receivedCertifyMessageIds[i] = vector;
                std::vector <int> vector2;
                this->receivedCertifyVoteBlockIds[i] = vector2;
            }
        }

        //check if node has not already received this soft vote
        int recMessageId = (*message)["messageId"].GetInt();
        for(auto messageId: this->receivedCertifyMessageIds[loopOfBlock]){
            if(messageId == recMessageId){
                //already receive
                return;
            }
        }
        this->receivedCertifyMessageIds[loopOfBlock].push_back(recMessageId);

        //send to others
        this->SendMessage(message, this->broadcastSocket);

        //check status
        int recBlockId = (*message)["blockId"].GetInt();

        this->receivedCertifyVoteBlockIds[loopOfBlock].push_back(recBlockId);
        int committeeSize = (this->nodeHelper->ListOfCommitteeMembers(loopOfBlock)).size();
        int counter = 0;
        int minimalCount = round(committeeSize * 0.6);

        for(auto blockId: this->receivedCertifyVoteBlockIds[loopOfBlock]){
            if(blockId == recBlockId){
                counter++;
            }
        }

        if(counter >= minimalCount){
            //add block
            rapidjson::Value &blockObj = (*message)["block"];
            rapidjson::StringBuffer sb;
            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
            blockObj.Accept(writer);
            std::string s = sb.GetString();
            rapidjson::Document messageBlock;
            messageBlock.Parse(s.c_str());
            Block *previousBlock = this->blockChain->GetTopBlock();
            Block *block = Block::FromJSON(&messageBlock,previousBlock,Ipv4Address("0.0.0.0"));
            if(this->blockChain->HasBlock(block)){
                return;
            }
//            NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " loop " << loopOfBlock <<  " adding new block");
            this->blockChain->AddBlock(block);
        }
    }

    void AlgorandNodeApp::PrintProcessInfo() {
        int myId = GetNode()->GetId();
        int recPropBlockCounter = 0;
        for(auto item: this->receivedProposedBlocks){
            recPropBlockCounter += item.size();
        }
        int recSoftVoteCounter = 0;
        for(auto item: this->receivedSoftVoteBlockIds){
            recSoftVoteCounter += item.size();
        }
        int recCertifyVoteCounter = 0;
        for(auto item: this->receivedCertifyVoteBlockIds){
            recCertifyVoteCounter += item.size();
        }
        int inPropseCommite = 0;
        for(int i = 0;i<=this->loopCounterProposedBlock;i++){
            for(auto item: this->nodeHelper->ListOfBlockProposals(i)){
                if(item == myId){
                    inPropseCommite++;
                    break;
                }
            }
        }
        int inVoteCommite = 0;
        for(int i = 0;i<=this->loopCounterCertifyVote;i++){
            for(auto item: this->nodeHelper->ListOfCommitteeMembers(i)){
                if(item == myId){
                    inVoteCommite++;
                    break;
                }
            }
        }

        NS_LOG_FUNCTION(this);
        // NS_LOG_INFO("");
        // NS_LOG_INFO(" Algorand App " << GetNode()->GetId());
        // NS_LOG_INFO("----------------------------------------------------------------------------------------   ");
        // NS_LOG_INFO(" Algorand Info: ");
        // NS_LOG_INFO(" Loop count (prop. blocks)    |  Loop count (vote)      |");
        // NS_LOG_INFO("            " << this->loopCounterProposedBlock+1 << "                |           " << this->loopCounterCertifyVote+1 << "            | ");
        // NS_LOG_INFO(" Rec. proposed blocks count   |  Rec. soft vote count   |  Rec. certify vote count  |");
        // NS_LOG_INFO("           " << recPropBlockCounter << "               |            " << recSoftVoteCounter << "         |             " << recCertifyVoteCounter << "        | ");
        // NS_LOG_INFO(" In propose (committee) count |  In vote committee count |  ");
        // NS_LOG_INFO("                " << inPropseCommite << "             |           " << inVoteCommite << "             | ");
        // NS_LOG_INFO(" Base Info: ");
        BlockChainNodeApp::PrintProcessInfo();

        double mean = 0.0;
        for( unsigned int i = 1; i < this->timeWaitingBlock.size(); i++ ){
            mean += this->timeWaitingBlock[ i ] - this->timeWaitingBlock[ 0 ];
        }
        mean = mean / ( this->timeWaitingBlock.size() - 1 );
        mean = mean ;//* 32.3;
        cout << "Block Propagation Time: " << std::to_string( mean ) << endl;

    }
}