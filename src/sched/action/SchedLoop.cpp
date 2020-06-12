//
// Created by Darwin Yuan on 2020/6/12.
//

#include <trans-dsl/sched/action/SchedLoop.h>
#include <trans-dsl/sched/concept/RuntimeContextAutoSwitch.h>
#include <trans-dsl/action/TransactionInfo.h>

TSL_NS_BEGIN

auto SchedLoop::checkError(LoopActionType type) {
   if(errorChecking && type == LoopActionType::ACTION) {
      cleanUpFailure();
      errorChecking = false;
   }

   if(!errorChecking && type != LoopActionType::ACTION) {
      errorChecking = true;
   }
}

auto SchedLoop::execOne(TransactionContext& context, LoopActionType type) -> Status {
   ActionStatus status = action->exec(context);
   if(type == LoopActionType::ACTION) {
      if(status.isFailed()) {
         reportFailure(status);
         // don't return, keep looping
         return Result::MOVE_ON;
      } else if(status.isDone()){
         return Result::MOVE_ON;
      }
   }

   return status;
}

auto SchedLoop::execOnce(TransactionContext& context) -> Status {
   LoopActionType type{};
   while((action = getAction(sequence, type)) != nullptr) {
      checkError(type);
      if(type != LoopActionType::ACTION || !hasFailure()) {
         Status status = execOne(context, type);
         if(status != Result::MOVE_ON) {
            return status;
         }
      }

      ++sequence;
   }

   return Result::RESTART_REQUIRED;
}

auto SchedLoop::looping(TransactionContext& context) -> Status {
   int deadloopTimes = 0;
   while(1) {
      if(deadloopTimes > 1) {
         return Result::USER_FATAL_BUG;
      }

      Status status = execOnce(context);
      if(status != Result::RESTART_REQUIRED) {
         return status;
      }
      sequence = 0;
      errorChecking = true;
      ++deadloopTimes;
   }

   // never arrive here.
   return Result::FATAL_BUG;
}

#define AUTO_SWITCH()  RuntimeContextAutoSwitch __autoSwitch__{context, *this}

SchedLoop::SchedLoop()
   : RuntimeContext(true) {
}

auto SchedLoop::exec(TransactionContext& context) -> Status {
   AUTO_SWITCH();
   return looping(context);
}

auto SchedLoop::handleEvent_(TransactionContext& context, const Event& event) -> Status {
   if(action == nullptr) {
      return Result::FATAL_BUG;
   }

   ActionStatus status = action->handleEvent(context, event);
   if(status.isWorking()){
      return status;
   } else if(status.isFailed()) {
      reportFailure(status);
   }

   ++sequence;
   return looping(context);
}

auto SchedLoop::handleEvent(TransactionContext& context, const Event& event) -> Status {
   AUTO_SWITCH();
   return handleEvent_(context, event);
}

auto SchedLoop::stop(TransactionContext& context) -> Status {
   AUTO_SWITCH();
   return Result::FATAL_BUG;
}

auto SchedLoop::kill(TransactionContext& ) -> void {
}

TSL_NS_END
