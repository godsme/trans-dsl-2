//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_TRANSACTIONCONTEXT_H
#define TRANS_DSL_2_TRANSACTIONCONTEXT_H

#include <trans-dsl/tsl_ns.h>
#include <cub/dci/Role.h>

TSL_NS_BEGIN

struct TransactionInfo;

struct TransactionContext {
   HAS_ROLE(TransactionInfo);
};

TSL_NS_END

#endif //TRANS_DSL_2_TRANSACTIONCONTEXT_H
