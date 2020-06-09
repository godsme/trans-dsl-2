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

template<typename T_ACTION>
struct SYNC__ : SyncActionAdapter  {
   OVERRIDE(exec(TransactionContext& context) -> Status) {
      return check(action(context.ROLE(TransactionInfo)));
   }

private:
   T_ACTION action;
};

using SyncActionFunc = Status (*)(const TransactionInfo&);

template<SyncActionFunc V_ACTION>
struct CALL__ : SyncActionAdapter  {
   OVERRIDE(exec(TransactionContext& context) -> Status) {
      return check(V_ACTION(context.ROLE(TransactionInfo)));
   }
};

#define __sync(action) TSL_NS::SYNC__<action>
#define __call(func) TSL_NS::CALL__<func>

TSL_NS_END

#endif //TRANS_DSL_2_SYNCACTIONHELPER_H
