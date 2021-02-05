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

   return ROLE(RelativeTimer).start(*timerInfo, context);
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::stopAction(TransactionContext& context) -> Status {
    auto status = ROLE(SchedAction).stop(context, Result::FATAL_BUG);
    state = (status == Result::CONTINUE) ? State::STOPPING : State::DONE;
    return status;
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::exec(TransactionContext& context)  -> Status {
   unlikely_branch
   if(unlikely(state != State::INIT)) return Result::FATAL_BUG;

   unlikely_branch
   if(auto status = ROLE(SchedAction).exec(context); unlikely(!is_working_status(status))) {
      state = State::DONE;
      return status;
   } else {
      unlikely_branch
      if(auto status = startTimer(context); unlikely(cub::is_failed_status(status))) {
          return stopAction(context) == Result::CONTINUE ? Result::CONTINUE : status;
      }
      state = State::WORKING;
      checkInternalError(context);
      return Result::CONTINUE;
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::handleEventTimeout(TransactionContext& context, Event const& event) -> Status {
   auto status = ROLE(SchedAction).handleEvent(context, event);
   likely_branch
   if(is_working_status(status)) {
      return status;
   }

   if(status == Result::SUCCESS || (status == Result::FORCE_STOPPED && !externalForceStopped)) {
      status = Result::TIMEOUT;
   }

   state = State::DONE;

   return status;
}

/////////////////////////////////////////////////////////////////////////////////////////
auto SchedTimeGuard::handleEvent(TransactionContext& context, Event const& event) -> Status {
   switch (state) {
   likely_branch
   case State::WORKING: [[fallthrough]];
   likely_branch
   case State::STOPPING: {
      auto status = ROLE(SchedAction).handleEvent(context, event);
      likely_branch
      if(likely(Result::__WORKING_STATUS_BEGIN & status)) {
         // Timeout is an exception, we test it unless the decorated action
         // did accept an event. This would speed-up the happy path, although
         // it might slow down the exceptional case (i.e, when it's actually the
         // expected TIMEOUT event)
         // (might be less helpful in multi-thread case)
         unlikely_branch
         if (unlikely(status == Result::UNKNOWN_EVENT && ROLE(RelativeTimer).matches(event))) {
            return onTimeout(context);
         }
      } else {
         ROLE(RelativeTimer).stop();
         state = State::DONE;
      }

      return status;
   }
   case State::TIMEOUT:
      return handleEventTimeout(context, event);
   unlikely_branch
   default:
      return Result::FATAL_BUG;
   }
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
   likely_branch
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
auto SchedTimeGuard::stop(TransactionContext& context, Status cause)  -> Status {
   switch (state) {
   likely_branch
   case State::WORKING:
      externalForceStopped = true;
      return stop_(context, cause);
   case State::STOPPING: [[fallthrough]];
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
