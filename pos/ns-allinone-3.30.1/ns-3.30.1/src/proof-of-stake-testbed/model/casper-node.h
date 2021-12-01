#ifndef PROJECT_ALGORAND_NODE_H
#define PROJECT_ALGORAND_NODE_H

#include "blockchain-node.h"
#include "ns3/event-id.h"
#include "casper-helper.h"

namespace ns3 {

    class CasperNodeApp : public BlockChainNodeApp {
    protected:
        CasperHelper *nodeHelper;

        bool HandleCustomRead(Ptr <Packet> packet, Address from, std::string receivedData, rapidjson::Document *document) override;

        void StartApplication(void);

        void StopApplication(void);

    public:
        CasperNodeApp(CasperHelper *nodeHelper);

        /**
         * Handle new received transaction
         * @param receivedData
         */
        void ReceiveNewTransaction(rapidjson::Document *message) override;

        void PrintProcessInfo();
    };
}

#endif //PROJECT_ALGORAND_NODE_H
