//
// Created by Darwin Yuan on 2020/6/12.
//

#include <trans-dsl/sched/action/SchedLoop.h>
#include <trans-dsl/sched/domain/RuntimeContextAutoSwitch.h>
#include <trans-dsl/action/TransactionInfo.h>
#include <trans-dsl/tsl_config.h>
#include <trans-dsl/utils/AssertionHelper.h>

TSL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::execAction(TransactionContext& context) -> Status {
   if(inPredSegment) {
      cleanUpFailure();
      inPredSegment = false;
   }

   if(hasFailure()) {
      // skip the action if an error occurred in this action segment.
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
   if(!inPredSegment) inPredSegment = true;
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
      EXPECT_STATUS(execEntry(context, isAction), Result::MOVE_ON);
   }

   return Result::RESTART_REQUIRED;
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::looping(TransactionContext& context) -> Status {
   auto allowedMaxTimes = getMaxTime();
   while(1) {
      EXPECT_STATUS(loopOnce(context), Result::RESTART_REQUIRED);

      index = 0;
      inPredSegment = true;

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
   EXPECT_STATUS(attachToParent(context), Result::SUCCESS);

   AUTO_SWITCH();
   return getResult(looping(context));
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::handleEvent_(TransactionContext& context, Event const& event) -> Status {
   Status status = action->handleEvent(context, event);
   if (is_working_status(status)) {
      return status;
   }

   if(cub::is_failed_status(status)) {
      reportFailure(status);
   }

   return looping(context);
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::handleEvent(TransactionContext& context, Event const& event) -> Status {
   BUG_CHECK(action != nullptr);

   AUTO_SWITCH();
   return getResult(handleEvent_(context, event));
}

////////////////////////////////////////////////////////////////////////////////////////
auto SchedLoop::stop(TransactionContext& context, Status cause) -> Status {
   BUG_CHECK(action != nullptr);

   unlikely_branch
   if(unlikely(stopping)) {
      return Result::CONTINUE;
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
