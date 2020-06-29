//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_SCHEDSYNCACTION_H
#define TRANS_DSL_2_SCHEDSYNCACTION_H

#include <trans-dsl/sched/domain/SchedAction.h>
#include <trans-dsl/utils/ActionStatus.h>

TSL_NS_BEGIN

struct SchedSyncAction : SchedAction  {
   OVERRIDE(handleEvent(TransactionContext&, Event const&) -> Status) {
      return Result::FATAL_BUG;
   }
   OVERRIDE(kill(TransactionContext&, Status) -> void) {}
   OVERRIDE(stop(TransactionContext&, Status) -> Status) {
      return Result::FATAL_BUG;
   }

protected:
   Status check(Status status) {
      return status == Result::CONTINUE ? Result::FATAL_BUG : status;
   }
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDSYNCACTION_H
