//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_SYNCACTION_H
#define TRANS_DSL_2_SYNCACTION_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/concept/SchedAction.h>
#include <trans-dsl/utils/ActionStatus.h>

TSL_NS_BEGIN

struct SyncAction : SchedAction  {
   OVERRIDE(handleEvent(TransactionContext&, Event&) -> Status) {
      return Result::FATAL_BUG;
   }
   OVERRIDE(kill(TransactionContext&, Status)        -> void) {}
   OVERRIDE(stop(TransactionContext&, Status) -> Status) {
      return Result::SUCCESS;
   }

protected:
   Status check(ActionStatus status) {
      return status.isWorking() ? Result::FATAL_BUG : status;
   }
};

TSL_NS_END

#endif //TRANS_DSL_2_SYNCACTION_H
