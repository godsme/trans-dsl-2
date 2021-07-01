//
// Created by Darwin Yuan on 2020/6/13.
//

#ifndef TRANS_DSL_2_SCHEDCONCURRENT_H
#define TRANS_DSL_2_SCHEDCONCURRENT_H

#include <trans-dsl/sched/domain/SchedAction.h>
#include <trans-dsl/sched/domain/RuntimeContext.h>
#include <trans-dsl/utils/SeqInt.h>

TSL_NS_BEGIN

struct SchedConcurrent
   : private RuntimeContext
   , SchedAction  {

   OVERRIDE(exec(TransactionContext&)                      -> Status);
   OVERRIDE(handleEvent(TransactionContext&, Event const&) -> Status);
   OVERRIDE(stop(TransactionContext&, Status cause)        -> Status);
   OVERRIDE(kill(TransactionContext&, Status cause)        -> void);

private:
   auto startUp(TransactionContext&) -> Status;
   auto hasWorkingChildren(SeqInt from) const;
   auto cleanUp(TransactionContext& context, Status failStatus) -> Status;
   auto handleEvent_(TransactionContext&, Event const&) -> Status;
   auto handleEvent__(TransactionContext& context, Event const& event) -> void;
   auto hasReportedError() const -> bool;
   auto notWorking() -> bool;

public:
   enum class State : uint8_t {
      Idle,
      Working,
      Stopping,
      Done
   };

protected:
    constexpr static bool Children_State_Required = true;

private:
   SeqInt total = 0;
   State state = State::Idle;

private:
   ABSTRACT(getNumOfActions() const -> SeqInt);
   ABSTRACT(getChildren() -> State*);
   ABSTRACT(getChildren() const -> State const*);
   ABSTRACT(get(SeqInt index) -> SchedAction*);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDCONCURRENT_H
