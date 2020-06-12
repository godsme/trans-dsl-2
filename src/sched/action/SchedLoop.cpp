//
// Created by Darwin Yuan on 2020/6/12.
//

#include <trans-dsl/sched/action/SchedLoop.h>

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

      Status status = execOne(context, type);
      if(status != Result::MOVE_ON) {
         return status;
      }

      ++sequence;
   }

   return Result::RESTART_REQUIRED;
}

auto SchedLoop::looping(TransactionContext& context) -> Status {
   while(1) {
      Status status = execOnce(context);
      if(status != Result::RESTART_REQUIRED) {
         return status;
      }
      sequence = 0;
      errorChecking = true;
   }

   // never arrive here.
   return Result::FATAL_BUG;
}

auto SchedLoop::exec(TransactionContext& context) -> Status {
   return looping(context);
}

auto SchedLoop::handleEvent(TransactionContext& context, const Event& event) -> Status {
   if(action == nullptr) {
      return Result::FATAL_BUG;
   }

   ActionStatus status = action->handleEvent(context, event);
   if(status.isDone()){
      ++sequence;
      return looping(context);
   }

   return status;
}

auto SchedLoop::stop(TransactionContext& ) -> Status {
   return Result::FATAL_BUG;
}

auto SchedLoop::kill(TransactionContext& ) -> void {
}

TSL_NS_END
