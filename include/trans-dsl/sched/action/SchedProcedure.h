//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_SCHEDPROCEDURE_H
#define TRANS_DSL_2_SCHEDPROCEDURE_H

#include <trans-dsl/sched/concept/SchedAction.h>
#include <trans-dsl/sched/concept/RuntimeContext.h>

TSL_NS_BEGIN

struct TransactionContext;

// 40 bytes
struct SchedProcedure
   : private RuntimeContext
   , SchedAction {

   OVERRIDE(exec(TransactionContext&)                      -> Status);
   OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status);
   OVERRIDE(stop(TransactionContext&)                      -> Status);
   OVERRIDE(kill(TransactionContext&)                      -> void);

private:
   auto exec_(TransactionContext&) -> Status;

private:
   struct State;
   State* state = nullptr;
   SchedAction* action = nullptr;

private:
   struct Idle;
   struct Working;
   struct Stopping;
   struct Final;
   struct Done;

private:
   template<typename T>
   Status gotoState(TransactionContext& context, Status status);

private:
   ABSTRACT(getAction() -> SchedAction*);
   ABSTRACT(getFinalAction() -> SchedAction*);
   ABSTRACT(isProtected() const -> bool);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDPROCEDURE_H
