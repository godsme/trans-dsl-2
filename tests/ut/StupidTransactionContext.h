//
// Created by Darwin Yuan on 2020/6/10.
//

#ifndef TRANS_DSL_2_STUPIDTRANSACTIONCONTEXT_H
#define TRANS_DSL_2_STUPIDTRANSACTIONCONTEXT_H

#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/action/TransactionInfo.h>
#include <trans-dsl/sched/domain/RuntimeContext.h>
#include <trans-dsl/porting/timer/PlatformSpecifiedTimerInfo.h>

struct StupidTransactionContext
   : TSL_NS::TransactionContext {

   StupidTransactionContext() {
      updateTimerInfo(&timerInfo);
   }

private:
   TSL_NS::PlatformSpecifiedTimerInfo timerInfo;
};

#endif //TRANS_DSL_2_STUPIDTRANSACTIONCONTEXT_H
