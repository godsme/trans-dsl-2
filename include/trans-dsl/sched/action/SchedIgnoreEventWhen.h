//
// Created by Darwin Yuan on 2021/4/26.
//

#ifndef TRANS_DSL_2_SCHEDIGNOREEVENTWHEN_H
#define TRANS_DSL_2_SCHEDIGNOREEVENTWHEN_H

#include <trans-dsl/sched/domain/SchedAction.h>

TSL_NS_BEGIN

struct SchedIgnoreEventWhen : SchedAction  {
    OVERRIDE(exec(TransactionContext&)                      -> Status);
    OVERRIDE(handleEvent(TransactionContext&, Event const&) -> Status);
    OVERRIDE(stop(TransactionContext&, Status cause)        -> Status);
    OVERRIDE(kill(TransactionContext&, Status cause)        -> void);

private:
    ABSTRACT(matches(EV_NS::EventId) const -> bool);
    USE_ROLE(SchedAction);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDIGNOREEVENTWHEN_H
