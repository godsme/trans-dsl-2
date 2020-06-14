//
// Created by Darwin Yuan on 2020/6/13.
//

#include <trans-dsl/sched/action/SchedConcurrent.h>
#include <event/concept/Event.h>
#include <trans-dsl/sched/concept/TransactionContext.h>
#include <iostream>

TSL_NS_BEGIN

#define IS_WORKING__(actionState) (actionState == State::Working || actionState == State::Stopping)
#define IS_CHILD_WORKING(i) IS_WORKING__(children[i])

///////////////////////////////////////////////////////////////////////////////
namespace {
   inline Status getLastError(Status lastError, Status status) {
      if(status == Result::FORCE_STOPPED && lastError != SUCCESS) {
         return lastError;
      }

      return status;
   }
}

inline auto SchedConcurrent::updateLastFailure(Status status) -> void {
   if(status == Result::FORCE_STOPPED && lastFailure != Result::SUCCESS) {
      return;
   }

   lastFailure = status;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::startUp(TransactionContext& context) -> Status {
   const auto total = getNumOfActions();
   auto hasWorkingChildren = false;
   for(SeqInt i=0; i < total; i++) {
      auto action = get(i);
      if(action == nullptr) {
         return Result::FATAL_BUG;
      }

      ActionStatus status = action->exec(context);
      if(status.isWorking()) {
         children[i] = State::Working;
         hasWorkingChildren = true;
      } else if(status.isDone()) {
         children[i] = State::Done;
      } else if(status.isFailed()) {
         children[i] = State::Done;
         return status;
      } else {
         return Result::FATAL_BUG;
      }
   }

   return hasWorkingChildren ? Result::CONTINUE : Result::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::cleanUp(TransactionContext& context, Status cause) -> Status {
   const auto total = getNumOfActions();
   auto hasWorking = false;

   for(SeqInt i=0; i < total; i++) {
      if(!IS_CHILD_WORKING(i)) continue;

      ActionStatus status = get(i)->stop(context, cause);
      if(status.isWorking()) {
         hasWorking = true;
      } else if(status.isFailed()) {
         updateLastFailure(status);
      }

      children[i] = status.isWorking() ? State::Stopping : State::Done;
   }

   state = hasWorking ? State::Stopping : State::Done;

   return IS_WORKING__(state) ? Result::CONTINUE : lastFailure;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::exec(TransactionContext& context) -> Status {
   if(state != State::Idle) {
      return FATAL_BUG;
   }

   ActionStatus status = startUp(context);
   if(status.isFailed()) {
      lastFailure = status;
      return cleanUp(context, lastFailure);
   }

   state = status.isWorking() ? State::Working : State::Done;

   return status;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::hasWorkingChildren(SeqInt from) const {
   const auto total = getNumOfActions();
   for(SeqInt i = from; i<total; i++) {
      if(IS_CHILD_WORKING(i)) return true;
   }

   return false;
}

auto SchedConcurrent::handleEvent__(TransactionContext& context, const Event& event) -> Status {
   const auto total = getNumOfActions();
   bool hasWorkingAction = false;

   SeqInt i = 0;
   for(; i < total; ++i) {
      if(!IS_CHILD_WORKING(i)) continue;

      ActionStatus status = get(i)->handleEvent(context, event);
      if(status.isWorking()) {
         hasWorkingAction = true;
      } else {
         children[i] = State::Done;
         if(status.isFailed()) updateLastFailure(status);
      }

      if(event.isConsumed()) break;
   }

   if(!hasWorkingAction && !hasWorkingChildren(i)) {
      state = State::Done;
   }

   return lastFailure;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::handleEvent_(TransactionContext& context, const Event& event) -> Status {
   ActionStatus lastError = handleEvent__(context, event);
   if (state == State::Working && lastError.isFailed()) {
      if(ActionStatus status = cleanUp(context, lastError); status.isFailed()) {
         return status;
      }
   }

   if(!IS_WORKING__(state)) return lastFailure;

   return event.isConsumed() ? Result::CONTINUE : Result::UNKNOWN_EVENT;
   //return lastError;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::handleEvent(TransactionContext& context, const Event& event) -> Status {
   if(!IS_WORKING__(state)) return Result::FATAL_BUG;

   return handleEvent_(context, event);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::stop(TransactionContext& context, Status cause) -> Status {
   switch (state) {
   case State::Working:
      return cleanUp(context, cause);
   case State::Stopping:
      return Result::CONTINUE;
   default:
      return Result::FATAL_BUG;
   }
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::kill(TransactionContext& context) -> void {
   if(!IS_WORKING__(state)) return;

   auto total = getNumOfActions();
   for(SeqInt i = 0; i<total; i++) {
      if(IS_CHILD_WORKING(i)) get(i)->kill(context);
   }

   state = State::Done;
}

TSL_NS_END
