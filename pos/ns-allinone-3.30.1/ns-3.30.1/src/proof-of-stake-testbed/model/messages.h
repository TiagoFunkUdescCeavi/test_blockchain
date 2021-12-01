//
// Created by honza on 19.3.20.
//

#ifndef PROJECT_MESSAGES_H
#define PROJECT_MESSAGES_H

namespace ns3 {
    enum MessageTypes{
        NEW_TRANSACTION,
        NEW_BLOCK,
        //protocol specific messages
        OUROBOROS_SEED,
        ALGORAND_BLOCK_PROPOSAL,
        ALGORAND_SOFT_VOTE,
        ALGORAND_CERTIFY_VOTE,
    };
}

#endif //PROJECT_MESSAGES_H
