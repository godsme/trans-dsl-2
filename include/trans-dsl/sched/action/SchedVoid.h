//
// Created by Darwin Yuan on 2020/6/16.
//

#ifndef TRANS_DSL_2_SCHEDVOID_H
#define TRANS_DSL_2_SCHEDVOID_H

#include <trans-dsl/sched/concept/SchedAction.h>
#include <trans-dsl/utils/ActionStatus.h>

TSL_NS_BEGIN

struct SchedVoid : SchedAction  {
   OVERRIDE(exec(TransactionContext& context) -> Status);
   OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status);
   OVERRIDE(kill(TransactionContext&, Status) -> void);
   OVERRIDE(stop(TransactionContext&, Status) -> Status);

private:
   USE_ROLE(SchedAction);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDVOID_H
