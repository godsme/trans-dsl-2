//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_SYNCACTIONHELPER_H
#define TRANS_DSL_2_SYNCACTIONHELPER_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/concept/SchedAction.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/concept/TransactionContext.h>
#include <trans-dsl/sched/concept/SyncAction.h>

TSL_NS_BEGIN

struct TransactionInfo;

using SyncActionFunc = Status (*)(const TransactionInfo&);

template<SyncActionFunc V_ACTION>
struct CALL__ : SyncActionAdapter  {
   OVERRIDE(exec(TransactionContext& context) -> Status) {
      ActionStatus r = V_ACTION(context.ROLE(TransactionInfo));
      return r.isWorking() ? Result::FATAL_BUG : r;
   }
};

#define __call(action) TSL_NS::CALL__<action>

TSL_NS_END

#endif //TRANS_DSL_2_SYNCACTIONHELPER_H
