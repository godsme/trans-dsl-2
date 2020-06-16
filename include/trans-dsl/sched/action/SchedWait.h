//
// Created by Darwin Yuan on 2020/6/16.
//

#ifndef TRANS_DSL_2_SCHEDWAIT_H
#define TRANS_DSL_2_SCHEDWAIT_H

#include <trans-dsl/sched/concept/SchedAction.h>

TSL_NS_BEGIN

struct SchedWait : SchedAction  {
   OVERRIDE(exec(TransactionContext& context) -> Status);
   OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status);
   OVERRIDE(kill(TransactionContext&, Status) -> void);
   OVERRIDE(stop(TransactionContext&, Status) -> Status);

private:
   ABSTRACT(getEventId() const -> EV_NS::EventId);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDWAIT_H
