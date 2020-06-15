//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_SCHEDTIMERGUARD_H
#define TRANS_DSL_2_SCHEDTIMERGUARD_H

#include <trans-dsl/sched/concept/SchedAction.h>

TSL_NS_BEGIN

struct SchedTimerGuard : SchedAction {
   OVERRIDE(exec(TransactionContext&)  -> Status);
   OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status);
   OVERRIDE(stop(TransactionContext&, Status cause)  -> Status);
   OVERRIDE(kill(TransactionContext&) -> void);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDTIMERGUARD_H
