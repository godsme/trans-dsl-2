//
// Created by Darwin Yuan on 2020/6/12.
//

#ifndef TRANS_DSL_2_SCHEDLOOP_H
#define TRANS_DSL_2_SCHEDLOOP_H

#include <trans-dsl/sched/concept/SchedAction.h>
#include <trans-dsl/sched/concept/RuntimeContext.h>

TSL_NS_BEGIN

struct TransactionContext;

enum class LoopActionType {
   ACTION,
   BREAK_PRED,
   CONTINUE_PRED
};

struct SchedLoop
   : private RuntimeContext
   , SchedAction {

   SchedLoop();

   OVERRIDE(exec(TransactionContext& ) -> Status);
   OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status);
   OVERRIDE(stop(TransactionContext& ) -> Status);
   OVERRIDE(kill(TransactionContext& ) -> void);

private:
   auto checkError(LoopActionType type);
   auto execOne(TransactionContext& context, LoopActionType type) -> Status;
   auto execOnce(TransactionContext&) -> Status;
   auto looping(TransactionContext& context) -> Status;
   auto handleEvent_(TransactionContext& context, const Event& event) -> Status;

private:
   SchedAction* action = nullptr;
   uint16_t sequence = 0;
   bool errorChecking = true;

private:
   ABSTRACT(getAction(uint16_t seq, LoopActionType&) -> SchedAction*);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDLOOP_H
