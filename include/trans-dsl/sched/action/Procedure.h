//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_PROCEDURE_H
#define TRANS_DSL_2_PROCEDURE_H

#include <trans-dsl/tsl_status.h>
#include <cub/dci/Role.h>
#include <trans-dsl/sched/concept/SchedAction.h>

TSL_NS_BEGIN

struct TransactionContext;
struct SchedAction;
struct FinalAction;

struct Procedure : SchedAction {
   OVERRIDE(exec(TransactionContext&)                      -> Status);
   OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status);
   OVERRIDE(stop(TransactionContext&)                      -> Status);
   OVERRIDE(kill(TransactionContext&)                      -> void);

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
};

TSL_NS_END

#endif //TRANS_DSL_2_PROCEDURE_H
