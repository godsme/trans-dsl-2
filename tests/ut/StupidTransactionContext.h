//
// Created by Darwin Yuan on 2020/6/10.
//

#ifndef TRANS_DSL_2_STUPIDTRANSACTIONCONTEXT_H
#define TRANS_DSL_2_STUPIDTRANSACTIONCONTEXT_H

#include <trans-dsl/sched/concept/TransactionContext.h>
#include <trans-dsl/action/TransactionInfo.h>
#include <trans-dsl/sched/concept/RuntimeContext.h>
#include <trans-dsl/porting/timer/PlatformSpecifiedTimerInfo.h>

struct StupidTransactionContext
   : TSL_NS::TransactionContext
   , TSL_NS::PlatformSpecifiedTimerInfo {
};

#endif //TRANS_DSL_2_STUPIDTRANSACTIONCONTEXT_H
