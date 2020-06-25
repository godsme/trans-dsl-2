//
// Created by Darwin Yuan on 2020/6/15.
//

#include <trans-dsl/sched/action/SchedTimeGuard.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/sched/domain/RelativeTimer.h>
#include <trans-dsl/sched/domain/TimerInfo.h>

TSL_NS_BEGIN

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::isStillWorking() const -> bool {
   switch (state) {
      case State::WORKING:
      case State::STOPPING:
      case State::TIMEOUT:
         return true;
      default:
         return false;
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::checkInternalError(TransactionContext& context) -> void {
   if(state == State::WORKING && context.hasFailure()) {
      state = State::STOPPING;
   }
}

auto SchedTimeGuard::startTimer(TransactionContext& context) -> Status {
   TimerInfo* timerInfo = context.getTimerInfo();
   if(timerInfo == nullptr) {
      return Result::FATAL_BUG;
   }

   return ROLE(RelativeTimer).start(*timerInfo);
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::exec(TransactionContext& context)  -> Status {
   if(state != State::INIT) return Result::FATAL_BUG;

   if(ActionStatus status = startTimer(context); status.isFailed()) {
      state = State::DONE;
      return status;
   }

   if(ActionStatus status = ROLE(SchedAction).exec(context); !status.isWorking()) {
      ROLE(RelativeTimer).stop();
      state = State::DONE;
      return status;
   }

   state = State::WORKING;

   checkInternalError(context);

   return Result::CONTINUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::handleEvent_(TransactionContext& context, Event const& event) -> Status {
   ActionStatus status = ROLE(SchedAction).handleEvent(context, event);
   if(status.isWorking()) {
      checkInternalError(context);
      return status;
   }

   if(state != State::TIMEOUT) {
      ROLE(RelativeTimer).stop();
   } else if(status.isDoneOrForceStopped()) {
      status = Result::TIMEDOUT;
   }

   state = State::DONE;

   return status;
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::stop_(TransactionContext& context, Status cause)  -> Status {
   if(state != State::WORKING) return Result::CONTINUE;

   ActionStatus status = ROLE(SchedAction).stop(context, cause);
   state = status.isWorking() ? State::STOPPING : State::DONE;

   if(state == State::DONE) {
      ROLE(RelativeTimer).stop();
   }

   return status;
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::handleEvent(TransactionContext& context, Event const& event) -> Status {
   if(!isStillWorking()) return FATAL_BUG;

   if(ROLE(RelativeTimer).matches(event)) {
      ActionStatus status = stop_(context, Result::TIMEDOUT);
      if(status.isDoneOrForceStopped()) return Result::TIMEDOUT;
      else if(status.isWorking()) state = State::TIMEOUT;
      return status;
   } else {
      return handleEvent_(context, event);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::stop(TransactionContext& context, Status cause)  -> Status {
   return stop_(context, cause);
}

/////////////////////////////////////////////////////////////////////////////////////////
auto  SchedTimeGuard::kill(TransactionContext& context, Status cause) -> void {
   if(!isStillWorking()) return;

   ROLE(RelativeTimer).stop();

   ROLE(SchedAction).kill(context, cause);
   state = State::DONE;
}

TSL_NS_END
