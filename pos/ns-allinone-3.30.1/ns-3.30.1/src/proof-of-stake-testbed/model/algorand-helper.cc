#include "ns3/log.h"
#include <random>
#include <chrono>
#include "ns3/internet-module.h"
#include <stdlib.h>
#include "algorand-helper.h"
#include "constants.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE ("AlgorandHelper");

    AlgorandHelper::AlgorandHelper(int countOfNodes, long int totalStack): NodeHelper(countOfNodes,totalStack) {
        //rnd generator
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator (seed);
        this->generator = generator;
        //poisson - transaction
        if((constants.algorandPoissonDistributionMeanCount > constants.numberOfNodes/2) || constants.algorandPoissonDistributionMeanCount < 2){
            NS_FATAL_ERROR("Critical value of algorandPoissonDistributionMeanCount");
        }
        std::poisson_distribution<int> d1(constants.algorandPoissonDistributionMeanCount);
        this->committeeSizeGenerationDistribution = d1;
    }

    void AlgorandHelper::CreateBlockProposal(int loopNumber) {
        if(loopNumber < (int)this->blockProposals.size()){
            return;
        }
        int lastSize = this->blockProposals.size();
        int committeeSize = this->committeeSizeGenerationDistribution(this->generator);
        if(committeeSize <= 3){
            committeeSize = constants.numberOfNodes;    //use full network as commitee
        }
        this->blockProposals.resize(loopNumber+1);
        for(int i=lastSize; i <= loopNumber; i++){
            int counter = 0;
            do{
                int committeeMemberNode = this->GetNodeRandomByStack();
                bool in = false;
                for(auto item: this->blockProposals[i]){
                    if(item == committeeMemberNode){
                        in = true;
                        break;
                    }
                }
                if(!in) {
                    this->blockProposals[i].push_back(committeeMemberNode);
                    counter++;
                }
            } while(counter < committeeSize);
        }
    }

    std::vector<int> AlgorandHelper::ListOfBlockProposals(int loopNumber)  {
        this->CreateBlockProposal(loopNumber);
        return this->blockProposals[loopNumber];
    }

    void AlgorandHelper::CreateCommitteeMembers(int loopNumber) {
        if(loopNumber < (int)this->committeeMembers.size()){
            return;
        }
        int lastSize = this->committeeMembers.size();
        int committeeSize = this->committeeSizeGenerationDistribution(this->generator);
        if(committeeSize <= 3){
            committeeSize = constants.numberOfNodes;    //use full network as commitee
        }
        this->committeeMembers.resize(loopNumber+1);
        for(int i=lastSize; i <= loopNumber; i++){
            int counter = 0;
            do{
                int committeeMemberNode = this->GetNodeRandomByStack();
                bool in = false;
                for(auto item: this->committeeMembers[i]){
                    if(item == committeeMemberNode){
                        in = true;
                        break;
                    }
                }
                if(!in) {
                    this->committeeMembers[i].push_back(committeeMemberNode);
                    counter++;
                }
            } while(counter < committeeSize);
        }
    }

    std::vector<int> AlgorandHelper::ListOfCommitteeMembers(int loopNumber) {
        this->CreateCommitteeMembers(loopNumber);
        return this->committeeMembers[loopNumber];
    }
}