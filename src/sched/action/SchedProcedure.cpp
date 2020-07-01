//
// Created by Darwin Yuan on 2020/6/7.
//

#include <trans-dsl/sched/action/SchedProcedure.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/domain/SchedAction.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/sched/domain/RuntimeContextAutoSwitch.h>
#include <trans-dsl/tsl_config.h>

TSL_NS_BEGIN

//////////////////////////////////////////////////////////////////////////////////
#define AUTO_SWITCH()  RuntimeContextAutoSwitch autoSwitch__{context, *this}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::gotoDone(Status status) -> Status {
   state = State::Done;

   unlikely_branch
   if(unlikely(cub::is_failed_status(status))) {
      return status;
   }

   return isProtected() ? Result::SUCCESS : getStatus();
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::gotoFinal(TransactionContext& context, Status status) -> Status {
   action = getFinalAction();
   unlikely_branch
   if(unlikely(action == nullptr)) {
      return Result::FATAL_BUG;
   }

   state = State::Final;

   unlikely_branch
   if(unlikely(cub::is_failed_status(status))) {
      reportFailure(status);
   }

   status = action->exec(context);
   if(is_working_status(status)) {
      return status;
   }

   return gotoDone(status);
}

//////////////////////////////////////////////////////////////////////////////////
inline auto SchedProcedure::workingStateCheck() -> void {
   unlikely_branch
   if (unlikely(getStatus() != Result::SUCCESS)) {
      state = State::Stopping;
   }
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::exec_(TransactionContext& context) -> Status {
   action = getAction();
   unlikely_branch
   if(unlikely(action == nullptr)) {
      return Result::FATAL_BUG;
   }

   auto status = action->exec(context);
   likely_branch
   if(likely(status == Result::CONTINUE)) {
      state = State::Working;
      workingStateCheck();
      return status;
   }

   return gotoFinal(context, status);
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::exec(TransactionContext& context) -> Status {
   unlikely_branch
   if (unlikely(state != State::Idle)) {
      return Result::FATAL_BUG;
   }

   auto status = attachToParent(context);
   unlikely_branch
   if(unlikely(status != Result::SUCCESS)) {
      return status;
   }

   sandbox = isProtected();

   AUTO_SWITCH();
   return exec_(context);
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::handleEvent_(TransactionContext& context, Event const& event) -> Status {
   auto status = action->handleEvent(context, event);
   likely_branch
   if(likely(Result::__WORKING_STATUS_BEGIN & status)) {
      if(status == Result::CONTINUE && state == State::Working) {
         workingStateCheck();
      }
      return status;
   }

   switch (state) {
      likely_branch
      case State::Working:
         [[fallthrough]];
      case State::Stopping:
         return gotoFinal(context, status);
      case State::Final:
         return gotoDone(status);
      unlikely_branch
      default:
         return Result::FATAL_BUG;
   }
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::handleEvent(TransactionContext& context, Event const& event) -> Status {
   switch (state) {
      likely_branch
      case State::Working:
         [[fallthrough]];
      case State::Stopping:
         [[fallthrough]];
      case State::Final: {
         AUTO_SWITCH();
         return handleEvent_(context, event);
      }
      unlikely_branch
      default:
         return Result::FATAL_BUG;
   }
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::stop_(TransactionContext& context, Status cause) -> Status {
   Status status = action->stop(context, cause);
   if(is_working_status(status)) {
      state = State::Stopping;
      return status;
   }

   return gotoFinal(context, status);
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::stop(TransactionContext& context, Status cause) -> Status {
   switch (state) {
      likely_branch
      case State::Working: {
         AUTO_SWITCH();
         return stop_(context, cause);
      }
      case State::Stopping:
         [[fallthrough]];
      case State::Final:
         return Result::CONTINUE;
      unlikely_branch
      default:
         return FATAL_BUG;
   }
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::kill(TransactionContext& context, Status cause)  -> void {
   switch (state) {
      likely_branch
      case State::Working:
         [[fallthrough]];
      case State::Stopping:
         [[fallthrough]];
      case State::Final: {
         AUTO_SWITCH();
         action->kill(context, cause);
         action = nullptr;
         state = State::Done;
         break;
      }
      unlikely_branch
      default:
         break;
   }
}

TSL_NS_END