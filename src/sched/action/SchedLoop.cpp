//
// Created by Darwin Yuan on 2020/6/12.
//

#include <trans-dsl/sched/action/SchedLoop.h>
#include <trans-dsl/sched/domain/RuntimeContextAutoSwitch.h>
#include <trans-dsl/action/TransactionInfo.h>

TSL_NS_BEGIN

auto SchedLoop::checkError(bool isAction) {
   // while in error recovering mode, once meet the 1st action,
   // cleanup the failure.
   if(errorRecovering) {
      if(isAction) {
         cleanUpFailure();
         errorRecovering = false;
      }
   } else {
      // once meets the 1st pred, enters error recovering mode.
      if(isAction) {
         errorRecovering = true;
      }
   }
}

auto SchedLoop::checkActionStatus(ActionStatus status) -> Status {
   if (status.isFailed()) {
      reportFailure(status);
      return Result::MOVE_ON;
   } else if (status.isDone()) {
      return Result::MOVE_ON;
   }

   return status;
}

auto SchedLoop::execOne(TransactionContext& context, bool isAction) -> Status {
   // skipp all actions after a failure, until meet a pred.
   if(isAction && hasFailure()) {
      return Result::MOVE_ON;
   }

   auto status = action->exec(context);
   if(isAction) {
      return checkActionStatus(status);
   }

   return status;
}

auto SchedLoop::execOnce(TransactionContext& context) -> Status {
   bool isAction{};
   while((action = getAction(sequence, isAction)) != nullptr) {
      checkError(isAction);

      auto status = execOne(context, isAction);
      if(status != Result::MOVE_ON) {
         return status;
      }

      ++sequence;
   }

   return Result::RESTART_REQUIRED;
}

auto SchedLoop::looping(TransactionContext& context) -> Status {
   int loopTimes = 0;
   while(1) {
      auto status = execOnce(context);
      if(status != Result::RESTART_REQUIRED) {
         return status == Result::UNSPECIFIED ? getStatus() : status;
      }

      sequence = 0;
      errorRecovering = true;

      if(++loopTimes > getMaxTime()) {
         return Result::USER_FATAL_BUG;
      }

   }

   // never arrive here.
   return Result::FATAL_BUG;
}

#define AUTO_SWITCH()  RuntimeContextAutoSwitch __autoSwitch__{context, *this}

////////////////////////////////////////////////////////////////////////////////////////
SchedLoop::SchedLoop()
   : RuntimeContext(true) {
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::exec(TransactionContext& context) -> Status {
   auto status = attachToParent(context);
   if(status != Result::SUCCESS) {
      return status;
   }

   AUTO_SWITCH();
   return looping(context);
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::handleEvent_(TransactionContext& context, Event const& event) -> Status {
   ActionStatus status = action->handleEvent(context, event);
   if (status.isWorking() || stopping) {
      return status;
   } else if(status.isFailed()) {
      reportFailure(status);
   }

   ++sequence;
   return looping(context);
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::handleEvent(TransactionContext& context, Event const& event) -> Status {
   if (action == nullptr) {
      return Result::FATAL_BUG;
   }

   AUTO_SWITCH();
   return handleEvent_(context, event);
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::stop(TransactionContext& context, Status cause) -> Status {
   if(stopping || action == nullptr) {
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
