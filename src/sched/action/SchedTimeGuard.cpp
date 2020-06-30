//
// Created by Darwin Yuan on 2020/6/15.
//

#include <trans-dsl/sched/action/SchedTimeGuard.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/sched/domain/RelativeTimer.h>
#include <trans-dsl/sched/domain/TimerInfo.h>
#include <trans-dsl/tsl_config.h>

TSL_NS_BEGIN

/////////////////////////////////////////////////////////////////////////////////////////
inline auto SchedTimeGuard::isStillWorking() const -> bool {
   switch (state) {
      likely_branch
      case State::WORKING:
         return true;
      case State::STOPPING:
      case State::TIMEOUT:
         return true;
      default:
         return false;
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
inline auto SchedTimeGuard::checkInternalError(TransactionContext& context) -> void {
   unlikely_branch
   if(state == State::WORKING && context.hasFailure()) {
      state = State::STOPPING;
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::startTimer(TransactionContext& context) -> Status {
   auto timerInfo = context.getTimerInfo();
   unlikely_branch
   if(timerInfo == nullptr) {
      return Result::FATAL_BUG;
   }

   return ROLE(RelativeTimer).start(*timerInfo);
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::exec(TransactionContext& context)  -> Status {
   unlikely_branch
   if(state != State::INIT) return Result::FATAL_BUG;

   unlikely_branch
   if(auto status = startTimer(context); cub::is_failed_status(status)) {
      state = State::DONE;
      return status;
   }

   unlikely_branch
   if(auto status = ROLE(SchedAction).exec(context); !is_working_status(status)) {
      ROLE(RelativeTimer).stop();
      state = State::DONE;
      return status;
   } else {
      state = State::WORKING;
      checkInternalError(context);
      return Result::CONTINUE;
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::handleEvent_(TransactionContext& context, Event const& event) -> Status {
   auto status = ROLE(SchedAction).handleEvent(context, event);
   likely_branch
   if(is_working_status(status)) {
      //checkInternalError(context);
      return status;
   } else if(state != State::TIMEOUT) {
      ROLE(RelativeTimer).stop();
   } else unlikely_branch
   if(status == Result::SUCCESS || (status == Result::FORCE_STOPPED && !externalForceStopped)) {
      status = Result::TIMEOUT;
   }

   state = State::DONE;

   return status;
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::stop_(TransactionContext& context, Status cause)  -> Status {
   auto status = ROLE(SchedAction).stop(context, cause);
   if(status == Result::CONTINUE) {
      state = State::STOPPING;
   } else {
      state = State::DONE;
      ROLE(RelativeTimer).stop();
   }

   return status;
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::onTimeout(TransactionContext& context) -> Status {
   Status status = Result::CONTINUE;
   if(state == State::WORKING) {
      status = stop_(context, Result::TIMEOUT);
      likely_branch
      if(status == Result::SUCCESS || status == Result::FORCE_STOPPED) {
         return Result::TIMEOUT;
      }
   }

   if(status == Result::CONTINUE) {
      state = State::TIMEOUT;
   }

   return status;
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::handleEvent(TransactionContext& context, Event const& event) -> Status {
   unlikely_branch
   if(!isStillWorking()) return Result::FATAL_BUG;

   unlikely_branch
   if(ROLE(RelativeTimer).matches(event)) {
       return onTimeout(context);
   } else {
       return handleEvent_(context, event);
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::stop(TransactionContext& context, Status cause)  -> Status {
   switch (state) {
   likely_branch
   case State::WORKING:
      externalForceStopped = true;
      return stop_(context, cause);
   case State::STOPPING:
   case State::TIMEOUT:
      return Result::CONTINUE;
   unlikely_branch
   default:
      return Result::FATAL_BUG;
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
auto  SchedTimeGuard::kill(TransactionContext& context, Status cause) -> void {
   unlikely_branch
   if(!isStillWorking()) return;

   ROLE(RelativeTimer).stop();

   ROLE(SchedAction).kill(context, cause);
   state = State::DONE;
}

TSL_NS_END
