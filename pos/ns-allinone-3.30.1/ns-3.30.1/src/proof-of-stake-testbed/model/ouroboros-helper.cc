#include "ns3/log.h"
#include "ns3/internet-module.h"
#include <stdlib.h>
#include "ouroboros-helper.h"
#include "constants.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("OuroborosHelper");

    OuroborosHelper::OuroborosHelper(double slotSizeSeconds, int securityParameter, int countOfNodes, long int totalStack): NodeHelper(countOfNodes,totalStack) {
        this->slotSizeSeconds = slotSizeSeconds;
        this->slotsInEpoch = 10 * securityParameter;
    }

    int OuroborosHelper::GetSlotLeader(int slotNumber) {
        if(slotNumber >= (int)this->slotLeaders.size()){
            this->slotLeaders.resize(slotNumber+1,-1);
        }
        if(this->slotLeaders[slotNumber] == -1) {
            //generate leader
            this->slotLeaders[slotNumber] = this->GetNodeRandomByStack();
        }
        return this->slotLeaders[slotNumber];
    }

    double OuroborosHelper::GetSlotSizeSeconds() const {
        return this->slotSizeSeconds;
    }

    double OuroborosHelper::GetEpochSizeSeconds() const {
        return (this->slotSizeSeconds * this->slotsInEpoch);
    }

    int OuroborosHelper::GetSlotNumber() {
        double timeSeconds = Simulator::Now().GetSeconds();
        return int(timeSeconds / this->slotSizeSeconds);
    }

    int OuroborosHelper::GetEpochNumber() {
        double timeSeconds = Simulator::Now().GetSeconds();
        double calc = timeSeconds / (this->slotSizeSeconds * this->slotsInEpoch);
        return int(calc);
    }

    int OuroborosHelper::GetSlotsInEpoch() const {
        return this->slotsInEpoch;
    }
}
