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
#include "casper-node.h"
#include "constants.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("CasperNodeApp");
    NS_OBJECT_ENSURE_REGISTERED (CasperNodeApp);

    CasperNodeApp::CasperNodeApp (CasperHelper *nodeHelper) : BlockChainNodeApp(nodeHelper) {
        this->nodeHelper = nodeHelper;
    }

    void CasperNodeApp::StartApplication() {
        NS_LOG_FUNCTION(this);
        NS_LOG_INFO("Starting Casper App " << GetNode()->GetId());
        BlockChainNodeApp::StartApplication();

        //start events

    }

    void CasperNodeApp::StopApplication() {
        NS_LOG_FUNCTION(this);
        BlockChainNodeApp::StopApplication();
        //stop events
    }


    bool CasperNodeApp::HandleCustomRead(Ptr <Packet> packet, Address from, std::string receivedData, rapidjson::Document *document) {
        NS_LOG_FUNCTION(this);
//        NS_LOG_INFO("Node " << GetNode()->GetId() << " time " << Simulator::Now().GetSeconds() << " Total Received Data: " << receivedData);

        if (!document->IsObject()) {
            NS_LOG_WARN("The parsed packet is corrupted: " << receivedData);
            return false;
        }
        return false;
    }

    void CasperNodeApp::ReceiveNewTransaction(rapidjson::Document *message){
        NS_LOG_FUNCTION(this);
//        NS_LOG_INFO("At time " << Simulator::Now().GetSeconds() << " node " << GetNode()->GetId() << " receive new Transaction");
        BlockChainNodeApp::ReceiveNewTransaction(message);
    }

    void CasperNodeApp::PrintProcessInfo() {
        int myId = GetNode()->GetId();

        NS_LOG_FUNCTION(this);
        NS_LOG_INFO("");
        NS_LOG_INFO(" Casper App " << GetNode()->GetId());
        NS_LOG_INFO("----------------------------------------------------------------------------------------   ");
        NS_LOG_INFO(" Casper Info: ");
        NS_LOG_INFO(" Base Info: ");
        BlockChainNodeApp::PrintProcessInfo();
        NS_LOG_INFO("");
    }
}