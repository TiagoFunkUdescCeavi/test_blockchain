#ifndef PROJECT_NODE_HELPER_H
#define PROJECT_NODE_HELPER_H

#include <vector>

namespace ns3 {

    class NodeHelper {
    protected:
        std::vector<long int> stackSizes;
        long int totalStack;
        int countOfNodes;
        long int transactionIdsGenerator;
        void CreateAndSetStack();
        long int totalRecMessages;
        long int totalSendMessages;
    public:
        NodeHelper(int countOfNodes, long int totalStack);
        long int GetTotalStack();
        long int GetNodeStack(int nodeId);
        int GetStackCoinOwner(long int stackCoin);
        int GetNodeRandomByStack();
        void SendStack(int senderNodeId, int receiverNodeId, long int size);
        long int GenerateTransactionId();
        long int GetActualTransactionIdGeneratorValue();
        void AddTotalRecMessages();
        void AddTotalSendMessages();
        long int GetTotalRecMessages();
        long int GetTotalSendMessages();
        /**
         * Print info about status
         */
        virtual void PrintProcessInfo();
    };
}
#endif //PROJECT_NODE_HELPER_H
