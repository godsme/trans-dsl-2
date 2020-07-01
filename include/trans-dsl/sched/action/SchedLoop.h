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
   OVERRIDE(handleEvent(TransactionContext&, Event const&) -> Status);
   OVERRIDE(stop(TransactionContext&, Status) -> Status);
   OVERRIDE(kill(TransactionContext&, Status) -> void);

private:
   auto execAction(TransactionContext& context) -> Status;
   auto execEntry(TransactionContext& context, bool isAction) -> Status;
   auto execPred(TransactionContext& context) -> Status;
   auto loopOnce(TransactionContext &context) -> Status;
   auto looping(TransactionContext& context) -> Status;
   auto handleEvent_(TransactionContext& context, Event const& event) -> Status;
   auto checkActionStatus(Status status) -> Status;
   auto getResult(Status status) const -> Status;

private:
   SchedAction* action = nullptr;
   SeqInt index = 0;
   bool InPredZone = true;
   bool stopping = false;

private:
   ABSTRACT(getMaxTime() const -> uint32_t);
   ABSTRACT(getAction(SeqInt seq, bool& isAction) -> SchedAction*);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDLOOP_H
