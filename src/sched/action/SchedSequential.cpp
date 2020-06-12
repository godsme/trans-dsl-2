//
// Created by Darwin Yuan on 2020/6/8.
//

#include <trans-dsl/sched/action/SchedSequential.h>
#include <trans-dsl/utils/ActionStatus.h>

TSL_NS_BEGIN

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::forward(TransactionContext& context) -> Status {
   while((current = getNext(index++)) != nullptr) {
      ActionStatus status = current->exec(context);
      if(!status.isDone()) {
         return status;
      }
   }

   return Result::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::exec(TransactionContext& context) -> Status {
   if(stopped) return Result::FATAL_BUG;
   return forward(context);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::handleEvent(TransactionContext& context, const Event& event) -> Status {
   if(current == nullptr) {
      return stopped ? Result::FATAL_BUG : Result::UNKNOWN_EVENT;
   }

   ActionStatus status = current->handleEvent(context, event);
   if(!status.isDone()) {
      return status;
   }

   if(stopped) {
      current = nullptr;
      return status;
   }

   return forward(context);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::stop(TransactionContext& context) -> Status {
   if(current == nullptr) {
      return Result::FATAL_BUG;
   }

   if(stopped) {
      return Result::CONTINUE;
   }

   stopped = true;

   ActionStatus status = current->stop(context);
   if(status.isWorking()) {
      return status;
   }

   current = nullptr;

   return status;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::kill(TransactionContext& context) -> void {
   if(current != nullptr) {
      current->kill(context);
      current = nullptr;
      stopped = true;
   }
}


TSL_NS_END