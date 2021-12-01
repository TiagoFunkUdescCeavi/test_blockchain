
#ifndef PROJECT_ALGORAND_HELPER_H
#define PROJECT_ALGORAND_HELPER_H

#include "node-helper.h"
#include <vector>

namespace ns3 {

    class AlgorandHelper : public NodeHelper {
    private:
        std::vector<std::vector<int>> blockProposals;
        std::vector<std::vector<int>> committeeMembers;
        std::default_random_engine generator;
        std::poisson_distribution<int> committeeSizeGenerationDistribution;
        double blockProposalsPercentageSize;
        double committeePercentageSize;
    protected:
        void CreateBlockProposal(int loopNumber);
        void CreateCommitteeMembers(int loopNumber);
    public:
        AlgorandHelper(int countOfNodes, long int totalStack);
        std::vector<int> ListOfBlockProposals(int loopNumber);
        std::vector<int> ListOfCommitteeMembers(int loopNumber);
    };
}

#endif //PROJECT_ALGORAND_HELPER_H
