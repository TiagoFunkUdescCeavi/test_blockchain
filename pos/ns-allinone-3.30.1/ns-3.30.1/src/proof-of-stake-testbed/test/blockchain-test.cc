
#include "../model/blockchain.h"
#include "ns3/test.h"
#include "ns3/log.h"
#include "ns3/abort.h"
#include "ns3/config.h"
#include "ns3/string.h"

using namespace ns3;

class BlockChainTestCase : public TestCase {
public:
    BlockChainTestCase():TestCase("test") {

    }

    virtual void DoRun(void){
        BlockChain blockChain();
        std::cout << "ahoj" << std::endl;

        NS_TEST_ASSERT_MSG_EQ (0, 0, "Packets were lost !");
    }

};