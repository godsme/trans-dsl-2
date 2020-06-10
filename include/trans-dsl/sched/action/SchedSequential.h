//
// Created by Darwin Yuan on 2020/6/8.
//

#ifndef TRANS_DSL_2_SCHEDSEQUENTIAL_H
#define TRANS_DSL_2_SCHEDSEQUENTIAL_H

#include <trans-dsl/sched/concept/SchedAction.h>

TSL_NS_BEGIN

struct SchedSequential : SchedAction {
   OVERRIDE(exec(TransactionContext&)                -> Status);
   OVERRIDE(handleEvent(TransactionContext&, Event&) -> Status);
   OVERRIDE(stop(TransactionContext&, Status)        -> Status);
   OVERRIDE(kill(TransactionContext&, Status)        -> void);

private:
   auto forward(TransactionContext&) -> Status;

private:
   SchedAction* current = nullptr;
   bool stopped = false;

private:
   ABSTRACT(getNext() -> SchedAction*);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDSEQUENTIAL_H
