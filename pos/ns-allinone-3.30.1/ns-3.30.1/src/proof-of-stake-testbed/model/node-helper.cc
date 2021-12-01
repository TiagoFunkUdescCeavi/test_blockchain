#include "node-helper.h"
#include "ns3/log.h"
#include "ns3/internet-module.h"
#include <stdlib.h>

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("NodeHelper");

    NodeHelper::NodeHelper(int countOfNodes, long int totalStack) {
        this->countOfNodes = countOfNodes;
        this->totalStack = totalStack;
        this->transactionIdsGenerator = 0;
        this->totalRecMessages = 0;
        this->totalSendMessages = 0;
        this->CreateAndSetStack();
    }

    void NodeHelper::CreateAndSetStack() {
        NS_LOG_FUNCTION(this);
        //basic regular division of stack
//        for(int i=0;i<countOfNodes;i++){
//            this->stackSizes[i] = (long int)(totalStack/countOfNodes);
//        }
        // divide random, based on the percentage diff
        long int basicStack = (long int)(totalStack/countOfNodes);
        double percentageDiff = 68.0;                       // how much (in percentage) it can be stack difference between nodes
        double numberPercentageDiff = percentageDiff/100;
        long int maxDiff = (long int)(basicStack*numberPercentageDiff);
        long int minStack = basicStack - maxDiff;
        if(minStack < 0){
            minStack = 0;
        }
        long int maxStack = basicStack + maxDiff;
        if(maxStack > totalStack){
            maxStack = totalStack;
        }
        long int minMaxDiff = maxStack - minStack;
        if(minMaxDiff == 0){
            minMaxDiff = 1;
        }

        long int restOfTotalStack = totalStack;
        this->stackSizes.resize(countOfNodes);
        for(int i=0;i<countOfNodes;i++){
            long int nodeStack = rand() % minMaxDiff + minStack;
            if(nodeStack > restOfTotalStack){
                nodeStack = restOfTotalStack;
            }
            //manual setting of node stack
//            if(i == 0){
//                nodeStack = 10000000;
//            }
            this->stackSizes[i] = nodeStack;
            restOfTotalStack = restOfTotalStack - nodeStack;
        }
        if(restOfTotalStack > 0) {
            int rest = restOfTotalStack / countOfNodes;
            for (int i = 0; i < countOfNodes; i++) {
                this->stackSizes[i] = this->stackSizes[i] + rest;
            }
        }
    }

    long int NodeHelper::GetNodeStack(int nodeId) {
        return this->stackSizes[nodeId];
    }

    long int NodeHelper::GetTotalStack() {
        return this->totalStack;
    }

    int NodeHelper::GetStackCoinOwner(long int stackCoin) {
        if(stackCoin > this->totalStack){
            NS_FATAL_ERROR("StackCoin is not exist");
            return 0;
        }
        int stackCounter = 0;
        int node = 0;
        for(node = 0;node < (int)this->stackSizes.size(); node++){
            stackCounter += this->stackSizes[node];
            if(stackCoin <= stackCounter){
                return node;
            }
        }
        return this->stackSizes.back();
    }

    int NodeHelper::GetNodeRandomByStack(){
        int randomCoin = (rand() % this->totalStack);
        return this->GetStackCoinOwner(randomCoin);
    }

    void NodeHelper::SendStack(int senderNodeId, int receiverNodeId, long int size) {
        long int senderResult = this->stackSizes[senderNodeId] - size;
        if(senderResult < 0){
            NS_FATAL_ERROR("Can not send stack. Not enought of sender stack");
            return;
        }
        // Todo transaction (unique)
        this->stackSizes[senderNodeId] = senderResult;
        this->stackSizes[receiverNodeId] = this->stackSizes[receiverNodeId] + size;
    }

    long int NodeHelper::GenerateTransactionId() {
        this->transactionIdsGenerator++;
        return this->transactionIdsGenerator;
    }

    long int NodeHelper::GetActualTransactionIdGeneratorValue(){
        return this->transactionIdsGenerator;
    }

    void NodeHelper::AddTotalRecMessages(){
        this->totalRecMessages++;
    }

    void NodeHelper::AddTotalSendMessages(){
        this->totalSendMessages++;
    }

    long int NodeHelper::GetTotalRecMessages(){
        return this->totalRecMessages;
    }

    long int NodeHelper::GetTotalSendMessages(){
        return this->totalSendMessages;
    }

    void NodeHelper::PrintProcessInfo() {
        NS_LOG_FUNCTION(this);
        NS_LOG_INFO("");
        NS_LOG_INFO("----------------------------------------------------------------------------------------   ");
        NS_LOG_INFO(" HELPER INFO ");
        NS_LOG_INFO("----------------------------------------------------------------------------------------   ");
        NS_LOG_INFO(" Total send messages counter    |  Total rec messages counter     |");
        NS_LOG_INFO("            " << this->totalSendMessages << "                |           " << this->totalRecMessages  << "");
        NS_LOG_INFO("");
    }
}
