//
// Created by Darwin Yuan on 2020/6/13.
//

#ifndef TRANS_DSL_2_SCHEDCONCURRENT_H
#define TRANS_DSL_2_SCHEDCONCURRENT_H

#include <trans-dsl/sched/concept/SchedAction.h>
#include <trans-dsl/sched/concept/RuntimeContext.h>
#include <trans-dsl/utils/SeqInt.h>

TSL_NS_BEGIN

struct SchedConcurrent
   : private RuntimeContext
   , SchedAction  {

   OVERRIDE(exec(TransactionContext&)                      -> Status);
   OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status);
   OVERRIDE(stop(TransactionContext&, Status cause)        -> Status);
   OVERRIDE(kill(TransactionContext&)                      -> void);

private:
   auto startUp(TransactionContext&) -> Status;
   auto hasWorkingChildren(SeqInt from) const;
   auto cleanUp(TransactionContext& context, Status failStatus) -> Status;
   auto handleEvent_(TransactionContext&, const Event&) -> Status;
   auto handleEvent__(TransactionContext& context, const Event& event) -> void;
   auto checkReportingError(SeqInt i) -> void;
   auto hasReportedError() const -> bool;

private:
   enum class State : uint8_t {
      Idle,
      Working,
      Stopping,
      Done
   };

protected:
   enum {
      Max_Num_Of_Children = 7
   };

private:
   State state = State::Idle;
   State children[Max_Num_Of_Children]{};

private:
   ABSTRACT(getNumOfActions() const -> SeqInt);
   ABSTRACT(get(SeqInt index) -> SchedAction*);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDCONCURRENT_H
