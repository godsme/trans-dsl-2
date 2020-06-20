//
// Created by Darwin Yuan on 2020/6/12.
//

#ifndef TRANS_DSL_2_SCHEDLOOP_H
#define TRANS_DSL_2_SCHEDLOOP_H

#include <trans-dsl/sched/domain/SchedAction.h>
#include <trans-dsl/sched/domain/RuntimeContext.h>
#include <trans-dsl/utils/SeqInt.h>

TSL_NS_BEGIN

struct TransactionContext;

// 40 bytes
struct SchedLoop
   : private RuntimeContext
   , SchedAction {

   SchedLoop();

   OVERRIDE(exec(TransactionContext& ) -> Status);
   OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status);
   OVERRIDE(stop(TransactionContext&, Status) -> Status);
   OVERRIDE(kill(TransactionContext&, Status) -> void);

private:
   auto checkError(bool isAction);
   auto execOne(TransactionContext& context, bool isPred) -> Status;
   auto execOnce(TransactionContext&) -> Status;
   auto looping(TransactionContext& context) -> Status;
   auto handleEvent_(TransactionContext& context, const Event& event) -> Status;
   auto checkActionStatus(ActionStatus status) -> Status;

private:
   SchedAction* action = nullptr;
   uint16_t sequence = 0;
   bool errorRecovering = true;
   bool stopping = false;

private:
   ABSTRACT(getMaxTime() const -> uint32_t);
   ABSTRACT(getAction(SeqInt seq, bool& isAction) -> SchedAction*);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDLOOP_H
