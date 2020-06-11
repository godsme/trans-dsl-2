//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_SCHEDSYNCACTION_H
#define TRANS_DSL_2_SCHEDSYNCACTION_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/concept/SchedAction.h>
#include <trans-dsl/utils/ActionStatus.h>

TSL_NS_BEGIN

struct SchedSyncAction : SchedAction  {
   OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status) {
      return Result::FATAL_BUG;
   }
   OVERRIDE(kill(TransactionContext&)        -> void) {}
   OVERRIDE(stop(TransactionContext&) -> Status) {
      return Result::FATAL_BUG;
   }

protected:
   Status check(ActionStatus status) {
      return status.isWorking() ? Result::FATAL_BUG : status;
   }
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDSYNCACTION_H
