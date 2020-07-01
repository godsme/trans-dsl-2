//
// Created by Darwin Yuan on 2020/6/12.
//

#include <trans-dsl/sched/action/SchedLoop.h>
#include <trans-dsl/sched/domain/RuntimeContextAutoSwitch.h>
#include <trans-dsl/action/TransactionInfo.h>
#include <trans-dsl/tsl_config.h>

TSL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::execAction(TransactionContext& context) -> Status {
   if(InPredZone) {
      cleanUpFailure();
      InPredZone = false;
   }

   if(hasFailure()) {
      return Result::MOVE_ON;
   }

   auto status = action->exec(context);
   if (is_working_status(status)) {
      return status;
   }

   if (cub::is_failed_status(status)) {
      reportFailure(status);
   }

   return Result::MOVE_ON;
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::execPred(TransactionContext& context) -> Status {
   InPredZone = true;
   return action->exec(context);
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::execEntry(TransactionContext& context, bool isAction) -> Status {
   return isAction ? execAction(context) : execPred(context);
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::loopOnce(TransactionContext& context) -> Status {
   bool isAction{};
   while((action = getAction(index++, isAction)) != nullptr) {
      auto status = execEntry(context, isAction);
      if(status != Result::MOVE_ON) {
         return status;
      }
   }

   return Result::RESTART_REQUIRED;
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::looping(TransactionContext& context) -> Status {
   auto allowedMaxTimes = getMaxTime();
   while(1) {
      auto status = loopOnce(context);
      if(status != Result::RESTART_REQUIRED) {
         return status;
      }

      index = 0;
      InPredZone = true;

      if(unlikely(allowedMaxTimes-- == 0)) {
         break;
      }
   }

   return Result::USER_FATAL_BUG;
}

#define AUTO_SWITCH()  RuntimeContextAutoSwitch __autoSwitch__{context, *this}

////////////////////////////////////////////////////////////////////////////////////////
SchedLoop::SchedLoop()
   : RuntimeContext(true) {
}

////////////////////////////////////////////////////////////////////////////////////////
inline auto SchedLoop::getResult(Status status) const -> Status {
   return status == Result::UNSPECIFIED ? getStatus() : status;
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::exec(TransactionContext& context) -> Status {
   unlikely_branch
   if(auto status = attachToParent(context);
      unlikely(status != Result::SUCCESS)) {
      return status;
   }

   AUTO_SWITCH();
   return getResult(looping(context));
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::handleEvent_(TransactionContext& context, Event const& event) -> Status {
   Status status = action->handleEvent(context, event);
   if (is_working_status(status) || stopping) {
      return status;
   } else if(cub::is_failed_status(status)) {
      reportFailure(status);
   }

   return looping(context);
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::handleEvent(TransactionContext& context, Event const& event) -> Status {
   unlikely_branch
   if (unlikely(action == nullptr)) {
      return Result::FATAL_BUG;
   }

   AUTO_SWITCH();
   return getResult(handleEvent_(context, event));
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::stop(TransactionContext& context, Status cause) -> Status {
   unlikely_branch
   if(unlikely(stopping || action == nullptr)) {
      return Result::FATAL_BUG;
   }

   stopping = true;

   AUTO_SWITCH();
   return action->stop(context, cause);
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::kill(TransactionContext& context, Status cause) -> void {
   if(action != nullptr) {
      AUTO_SWITCH();
      action->kill(context, cause);
      action = nullptr;
   }
}

TSL_NS_END
