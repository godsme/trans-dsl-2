//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_SCHEDTIMERGUARD_H
#define TRANS_DSL_2_SCHEDTIMERGUARD_H

#include <trans-dsl/sched/concept/SchedAction.h>

TSL_NS_BEGIN

struct RelativeTimer;

struct SchedTimerGuard : SchedAction {
   OVERRIDE(exec(TransactionContext&)  -> Status);
   OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status);
   OVERRIDE(stop(TransactionContext&, Status cause) -> Status);
   OVERRIDE(kill(TransactionContext&, Status cause) -> void);

private:
   auto isStillWorking() const -> bool;
   auto handleEvent_(TransactionContext &, const Event &) -> Status;
   auto stop_(TransactionContext&, Status cause)  -> Status;
   auto checkInternalError(TransactionContext& context) -> void;
   auto startTimer(TransactionContext& context) -> Status;

private:
   enum class State {
      INIT,
      WORKING,  // normally working
      STOPPING, // action is stopped either by stop or by an internal error
                // but action is not done, timer doesn't timeout either.
      TIMEOUT,  // timeout either in WORKING or STOPPING state
                // but action is not done yet.
      DONE
   };
   State state = State::INIT;

private:
   USE_ROLE(RelativeTimer);
   USE_ROLE(SchedAction);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDTIMERGUARD_H
