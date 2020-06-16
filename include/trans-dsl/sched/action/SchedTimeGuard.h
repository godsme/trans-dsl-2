//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_SCHEDTIMEGUARD_H
#define TRANS_DSL_2_SCHEDTIMEGUARD_H

#include <trans-dsl/sched/domain/SchedAction.h>

TSL_NS_BEGIN

struct RelativeTimer;

struct SchedTimeGuard : SchedAction {
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

#endif //TRANS_DSL_2_SCHEDTIMEGUARD_H
