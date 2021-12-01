
#ifndef PROJECT_NODE_H
#define PROJECT_NODE_H

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
//#include "ns3/application.h"
#include "../../network/model/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "blockchain.h"
#include "messages.h"
#include "../utils/rsa.h"
#include "../../../rapidjson/document.h"
#include "../../../rapidjson/writer.h"
#include "../../../rapidjson/stringbuffer.h"
#include "node-helper.h"
#include <random>

using namespace std;

namespace ns3 {

    class Socket;
    class EventId;

    class BlockChainNodeApp : public Application {
    private:
        void Init(NodeHelper *nodeHelper);
    protected:
        Keys keys;                                                  //node RSA
        NodeHelper *nodeHelper;                                     // link to helper
        BlockChain *blockChain;                                      //node's blockchain
        std::vector<int> receivedTransactionsIds;                   //
        std::vector<Transaction *> receivedTransactions;              //
        Ptr<Socket> listenSocket;                                   //listening socket
        Ptr<Socket> broadcastSocket;                                //broadcastfa socket
        std::vector<Ipv4Address> nodesAddresses;                    //list of all nodes addresses
        std::map<Ipv4Address, Ptr<Socket>> nodesSockets;            //sockets to all nodes
        Address multicastLocal;                                     //local multicast address
        EventId nextEvent;                                          // next event to process
        EventId nextNewTransactionsEvent;                           // next event to generate transactions
        Ipv4InterfaceContainer netContainer;                        // container of whole network
        std::default_random_engine generator;                       // random generator
        std::poisson_distribution<int> transactionGenerationDistribution;
        std::poisson_distribution<int> feeGenerationDistribution;
        int countOfGeneratedTransactions;
        int highestNumberOfHops;
        int countOfReceivedHopsMessages;
        int roundNumberOfHops;

        /**
         * Override
         * Called when application started
         */
        virtual void StartApplication (void);

        /**
         * Override
         * Called when application finished
         */
        virtual void StopApplication (void);

        /**
         * Handle received packets
         * @param socket
         */
        void HandleRead (Ptr<Socket> socket);

        /**
         * Handle an incomming conneciton
         * @param socket
         * @param address
         */
        void HandleConnectionAccept(Ptr<Socket> socket, const Address& address);

        /**
         * handle conneciton end or error
         * @param socket
         */
        void HandleConnectionEnd(Ptr<Socket> socket);

        /**
         * There is implementation of customer packet read handle
         * return True - If packet is served by that method
         * return False - packet was not handled, should be handle by another method.
         * @param packet
         * @param from
         * @return
         */
        virtual bool HandleCustomRead(Ptr <Packet> packet, Address from, std::string receivedData, rapidjson::Document *document);

        /**
         * Implementation of general packet handle
         * @param packet
         * @param from
         */
        void HandleGeneralRead(Ptr <Packet> packet, Address from, std::string receivedData, rapidjson::Document *document);

        /**
         * Handle when receive new block
         * @param message
         */
        virtual void ReceiveBlock(rapidjson::Document *message);

        /**
         * Handle when receive more new blocks
         * @param blocks
         */
        void ReceiveBlocks(std::vector <Block> &blocks);

        /**
         * Handle new received transaction
         * @param receivedData
         */
        virtual void ReceiveNewTransaction(rapidjson::Document *message);

        /**
         * Sort receivedTransactions vector
         */
        void SortReceivedTransactionsByFee();

    public:
        /**
         * Constructor
         * @param nodeHelper
         */
        BlockChainNodeApp(NodeHelper *nodeHelper);
        /**
         * Constructor
         * @param netContainer
         * @param nodeHelper
         */
        BlockChainNodeApp(Ipv4InterfaceContainer netContainer,NodeHelper *nodeHelper);

        static TypeId GetTypeId (void);

        Ptr <Socket> GetListenPort(void) const;
        /**
         *
         * @param peers
         */
        void SetNodesAddresses(std::vector <Ipv4Address> &peers);
        /**
         *
         * @param message
         * @param outgoingSocket
         */
        void SendMessage(rapidjson::Document *message, Ptr<Socket> outgoingSocket);
        /**
         *
         * @param message
         * @param outgoingAddress
         */
        void SendMessage(rapidjson::Document *message, Address &outgoingAddress);
        /**
         * Generate and send new transactions
         */
        void GenerateSendTransactions();
        /**
         *
         * @return
         */
        int GetHighestNumberOfHops();
        /**
         *
         * @return
         */
        double GetRoundNumberOfHops();
        /**
         * Print info about node status
         */
        virtual void PrintProcessInfo();
    };
}

#endif //PROJECT_NODE_H

