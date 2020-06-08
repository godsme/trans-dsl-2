//
// Created by Darwin Yuan on 2020/6/8.
//

#include <trans-dsl/sched/action/SchedSequential.h>
#include <trans-dsl/utils/ActionStatus.h>

TSL_NS_BEGIN

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::forward(TransactionContext& context) -> Status {
   while((current = getNext()) != nullptr) {
      ActionStatus status = current->exec(context);
      if(!status.isDone()) {
         return status;
      }
   }

   return Result::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::exec(TransactionContext& context) -> Status {
   return forward(context);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::handleEvent(TransactionContext& context, Event& event) -> Status {
   if(current == nullptr) {
      return Result::UNKNOWN_EVENT;
   }

   ActionStatus status = current->handleEvent(context, event);
   if(!status.isDone()) {
      return status;
   }

   if(stopped) {
      current = nullptr;
      return finalStatus;
   }

   return forward(context);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::stop(TransactionContext& context, Status cause) -> Status {
   if(current == nullptr) {
      return Result::FATAL_BUG;
   }

   if(stopped) {
      return Result::CONTINUE;
   }

   stopped = true;
   finalStatus = cause;

   ActionStatus status = current->stop(context, cause);
   if(!status.isWorking()) {
      return status;
   }

   return finalStatus;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::kill(TransactionContext& context, Status cause) -> void {
   if(current != nullptr) {
      current->kill(context, cause);
      current = nullptr;
   }
}


TSL_NS_END