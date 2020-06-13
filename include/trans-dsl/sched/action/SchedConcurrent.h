//
// Created by Darwin Yuan on 2020/6/13.
//

#ifndef TRANS_DSL_2_SCHEDCONCURRENT_H
#define TRANS_DSL_2_SCHEDCONCURRENT_H

#include <trans-dsl/sched/concept/SchedAction.h>
#include <trans-dsl/sched/concept/RuntimeContext.h>
#include <trans-dsl/utils/SeqInt.h>

TSL_NS_BEGIN

struct SchedConcurrent : SchedAction  {
   OVERRIDE(exec(TransactionContext&)                      -> Status);
   OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status);
   OVERRIDE(stop(TransactionContext&)                      -> Status);
   OVERRIDE(kill(TransactionContext&)                      -> void);

private:
   auto startUp(TransactionContext&) -> Status;
   auto cleanUp_(TransactionContext& context, Status& lastError) -> Status;
   auto hasWorkingChildren(SeqInt from) const;
   auto cleanUp(TransactionContext& context, Status failStatus) -> Status;
   auto getFinalStatus(Status lastError, bool hasWorkingAction) -> Status;
   auto handleEvent_(TransactionContext&, const Event&) -> Status;

private:
   enum class State : uint8_t {
      Idle,
      Working,
      Stopping,
      Done
   };

private:
   RuntimeContext runtimeContext;
   State state = State::Idle;
   State children[7]{};

private:
   ABSTRACT(getNumOfActions() const -> SeqInt);
   ABSTRACT(get(SeqInt index) -> SchedAction*);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDCONCURRENT_H
