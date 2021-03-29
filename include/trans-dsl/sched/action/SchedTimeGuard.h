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
   OVERRIDE(handleEvent(TransactionContext&, Event const&) -> Status);
   OVERRIDE(stop(TransactionContext&, Status cause) -> Status);
   OVERRIDE(kill(TransactionContext&, Status cause) -> void);

private:
   auto isStillWorking() const -> bool;
   auto handleEventTimeout(TransactionContext& context, Event const& event) -> Status;
   auto stop_(TransactionContext&, Status cause, bool report)  -> Status;
   auto startTimer(TransactionContext& context) -> Status;
   auto onTimeout(TransactionContext& context) -> Status;
   auto stopOnTimeout(TransactionContext& context) -> Status;

private:
   enum class State : uint8_t {
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
    ABSTRACT(getTimeoutResult() const -> Status);
private:
   USE_ROLE(RelativeTimer);
   USE_ROLE(SchedAction);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDTIMEGUARD_H
