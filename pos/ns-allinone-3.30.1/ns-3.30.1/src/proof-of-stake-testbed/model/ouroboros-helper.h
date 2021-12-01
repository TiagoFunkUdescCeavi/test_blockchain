
#ifndef PROJECT_OUROBOROS_HELPER_H
#define PROJECT_OUROBOROS_HELPER_H

#include "node-helper.h"
#include <vector>

namespace ns3 {

    class OuroborosHelper : public NodeHelper {
        // https://docs.cardano.org/cardano/proof-of-stake/
    protected:
        std::vector<int> slotLeaders;
        double slotSizeSeconds;    //basic epoch size is about 20 seconds
        int slotsInEpoch;       //10 * security parameter
    public:
        OuroborosHelper(double slotSizeSeconds, int securityParameter, int countOfNodes, long int totalStack);
        int GetSlotLeader(int slotNumber);
        /**
         *
         * @return int Started on the 0 num
         */
        int GetSlotNumber();
        /**
         *
         * @return int Started on the 0 num
         */
        int GetEpochNumber();
        double GetSlotSizeSeconds() const;
        double GetEpochSizeSeconds() const;
        int GetSlotsInEpoch() const;

    };
}

#endif //PROJECT_OUROBOROS_HELPER_H
