#ifndef PROJECT_CONSTANSTS_H
#define PROJECT_CONSTANSTS_H
#include <stdint.h>

namespace ns3 {
    enum TransactionGenerationType{
        T_RAND,
        T_POISSON,
    };

    enum FeeGenerationType{
        F_RAND,
        F_POISSON,
    };

    class Constants{
    public:
        Constants();
        int maxTransactionsPerBlock;
        int maxTransactionPoolSize;
        uint32_t numberOfNodes;
        long int totalStack;
        double simulationTimeSeconds;
        //transaction generation
        TransactionGenerationType transactionGenerationType;
        double transPoissonDistributionMeanMiliSeconds;
        int randMaxTransactionGenerationTimeMiliSeconds;
        //stack generation
        FeeGenerationType feeGenerationType;
        double feePoissonDistributionMean;
        int randMaxFeeGeneration;

        //network specific
        int networkDecentralizedSizePerLocal;
        int networkDistributedCountOfConnections;

        //ouroboros
        double ouroborosSlotSizeSeconds;
        int ouroborosSecurityParameter;

        //algorand
        double algorandProposeCommittePercenategeSize;
        double algorandVoteCommittePercenategeSize;
        double algorandWaitingProposedBlockSeconds;
        double algorandPoissonDistributionMeanCount;
    };

    extern Constants constants;
}

#endif //PROJECT_CONSTANSTS_H
