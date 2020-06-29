//
// Created by Darwin Yuan on 2020/6/8.
//

#include <trans-dsl/sched/action/SchedSequential.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/tsl_config.h>

TSL_NS_BEGIN

///////////////////////////////////////////////////////////////////////////////
inline auto SchedSequential::getFinalStatus(Status status) -> Status {
   unlikely_branch
   if(stopped && status == Result::SUCCESS) {
      return getNumOfActions() == index + 1 ? Result::SUCCESS : Result::FORCE_STOPPED;
   }

   return status;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::forward(TransactionContext& context) -> Status {
   unlikely_branch
   while((current = getNext(index++)) != nullptr) {
      Status status = current->exec(context);
      if(status != Result::SUCCESS) {
         return status;
      }
   }

   return Result::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::exec(TransactionContext& context) -> Status {
   unlikely_branch
   if(stopped) return Result::FATAL_BUG;
   return forward(context);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::handleEvent_(TransactionContext& context, Event const& event) -> Status {
   unlikely_branch
   if(current == nullptr)  {
     return stopped ? Result::FATAL_BUG : Result::UNKNOWN_EVENT;
   }

   Status status = current->handleEvent(context, event);
   if(status != Result::SUCCESS) {
      return status;
   }

   unlikely_branch
   if(stopped) {
      current = nullptr;
      return status;
   }

   return forward(context);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::handleEvent(TransactionContext& context, Event const& event) -> Status {
   return getFinalStatus(handleEvent_(context, event));
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::stop(TransactionContext& context, Status cause) -> Status {
   unlikely_branch
   if( current == nullptr) {
      return Result::FATAL_BUG;
   }

   unlikely_branch
   if(stopped) {
      return Result::CONTINUE;
   }

   stopped = true;

   Status status = current->stop(context, cause);
   if(status == Result::CONTINUE) {
      return status;
   }

   current = nullptr;

   return getFinalStatus(status);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::kill(TransactionContext& context, Status cause) -> void {
   if(current != nullptr) {
      current->kill(context, cause);
      current = nullptr;
      stopped = true;
   }
}


TSL_NS_END