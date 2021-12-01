
#ifndef PROJECT_BLOCKCHAIN_H
#define PROJECT_BLOCKCHAIN_H

#include <vector>
#include "ns3/address.h"
#include "ns3/ipv4-address.h"
#include <algorithm>
#include "../../../rapidjson/document.h"


namespace ns3 {
    class Ipv4Address;

    class Transaction{
    private:
        long int id;
        int senderId;
        int receiverId;
        double transactionFee;
    public:
        Transaction(long int id, int senderId, int receiverId);
        int GetReceiverId();
        int GetSenderId();
        int GetId();
        void SetTransactionFee(double fee);
        double GetTransactionFee();
        rapidjson::Document ToJSON();
        rapidjson::Value ToJSON(rapidjson::Document *message);
        static Transaction *FromJSON(rapidjson::Document *document);
    };

    class Block {
    private:
        long int id;
        int blockHeight;
        int validatorId;
        Block *previousBlock;
        double timeCreated;
        double timeReceived;
        Ipv4Address receivedFrom;
        std::vector <Transaction *> transactions;
        int fullBlockCounter;
        int transactionCounter;
        int loopNum;
        double allTransactionsFee;
    public:
        Block(int blockHeight, int validatorId, Block *previousBlock, double timeCreated, double timeReceived, Ipv4Address receivedFrom);
        int GetBlockHeight() ;
        int GetBlockSize();
        void SetBlockSize(int blockSize);
        long int GetId();
        void SetId(long int id);
        bool IsBlockFull();
        int GetValidatorId() ;
        Block *GetPreviousBlock() ;
        double GetTimeCreated() ;
        double GetTimeReceived();
//        IPv4Address GetReceivedFrom() const;
        void AddTransaction(Transaction *transaction);
        std::vector <Transaction *> GetTransactions();
        std::vector <Transaction *> GetTransactionsByReceiver(int receiverId);
        std::vector <Transaction *> GetTransactionsBySender(int senderId);
        bool IsSameAs(Block *block);
        bool IsExactSameAs(Block *block);
        rapidjson::Document ToJSON();
        static Block *FromJSON(rapidjson::Document *document, Block *previousBlock, Ipv4Address receivedFrom);
        void SetFullBlockCounter(int counter);
        int GetFullBlockCounter();
        void SetLoopNumber(int loopNum);
        int GetLoopNumber();
        double GetAllTransactionsFee();
        void SetAllTransactionsFee(double fee);
    };

    class BlockChain {
    private:
        std::vector <std::vector<Block *>> blocks;
        int totalCountOfBlocks;
    public:
        BlockChain();
        int GetTotalCountOfBlocks();
        Block *GetTopBlock();
        int GetBlockchainHeight();
        bool HasBlock(Block *block);
        void AddBlock(Block *block);
        void PrintInfo();
    };
}

#endif //PROJECT_BLOCKCHAIN_H
