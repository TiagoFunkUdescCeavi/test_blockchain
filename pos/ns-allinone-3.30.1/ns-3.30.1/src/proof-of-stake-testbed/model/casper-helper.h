
#ifndef PROJECT_ALGORAND_HELPER_H
#define PROJECT_ALGORAND_HELPER_H

#include "node-helper.h"
#include <vector>

namespace ns3 {

    class CasperHelper : public NodeHelper {
    public:
        CasperHelper(int countOfNodes, long int totalStack);
    };
}

#endif //PROJECT_ALGORAND_HELPER_H
