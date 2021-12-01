#include "ns3/log.h"
#include "ns3/internet-module.h"
#include <stdlib.h>
#include "casper-helper.h"
#include "constants.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("CasperHelper");

    CasperHelper::CasperHelper(int countOfNodes, long int totalStack): NodeHelper(countOfNodes,totalStack) {

    }

}