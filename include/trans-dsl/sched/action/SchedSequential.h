//
// Created by Darwin Yuan on 2020/6/8.
//

#ifndef TRANS_DSL_2_SCHEDSEQUENTIAL_H
#define TRANS_DSL_2_SCHEDSEQUENTIAL_H

#include <trans-dsl/sched/concept/SchedAction.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/utils/SeqInt.h>

TSL_NS_BEGIN

// 24 bytes
struct SchedSequential : SchedAction {
   OVERRIDE(exec(TransactionContext&)                      -> Status);
   OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status);
   OVERRIDE(stop(TransactionContext&, Status cause)        -> Status);
   OVERRIDE(kill(TransactionContext&, Status cause)        -> void);

private:
   auto forward(TransactionContext&) -> Status;
   auto getFinalStatus(ActionStatus status) -> Status;
   auto handleEvent_(TransactionContext& context, const Event& event) -> Status;

private:
   SchedAction* current = nullptr;
   SeqInt index = 0;
   bool stopped = false;

private:
   ABSTRACT(getNumOfActions() -> SeqInt);
   ABSTRACT(getNext(SeqInt index) -> SchedAction*);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDSEQUENTIAL_H
