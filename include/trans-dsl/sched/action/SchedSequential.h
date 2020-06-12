//
// Created by Darwin Yuan on 2020/6/8.
//

#ifndef TRANS_DSL_2_SCHEDSEQUENTIAL_H
#define TRANS_DSL_2_SCHEDSEQUENTIAL_H

#include <trans-dsl/sched/concept/SchedAction.h>

TSL_NS_BEGIN

// 24 bytes
struct SchedSequential : SchedAction {
   OVERRIDE(exec(TransactionContext&)                      -> Status);
   OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status);
   OVERRIDE(stop(TransactionContext&)                      -> Status);
   OVERRIDE(kill(TransactionContext&)                      -> void);

private:
   auto forward(TransactionContext&) -> Status;

private:
   SchedAction* current = nullptr;
   uint16_t index = 0;
   bool stopped = false;

private:
   ABSTRACT(getNext(uint16_t index) -> SchedAction*);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDSEQUENTIAL_H
