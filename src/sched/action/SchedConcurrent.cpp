//
// Created by Darwin Yuan on 2020/6/13.
//

#include <trans-dsl/sched/action/SchedConcurrent.h>
#include <event/concept/Event.h>
#include <trans-dsl/sched/concept/TransactionContext.h>

TSL_NS_BEGIN

#define IS_WORKING__(actionState) (actionState == State::Working || actionState == State::Stopping)
#define IS_CHILD_WORKING(i) IS_WORKING__(children[i])

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
         reportFailure(status);
      }

      children[i] = status.isWorking() ? State::Stopping : State::Done;
   }

   state = hasWorking ? State::Stopping : State::Done;

   return IS_WORKING__(state) ? Result::CONTINUE : finalStatus;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::exec(TransactionContext& context) -> Status {
   if(state != State::Idle) {
      return FATAL_BUG;
   }

   ActionStatus status = startUp(context);
   if(status.isFailed()) {
      reportFailure(status);
      return cleanUp(context, finalStatus);
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

auto SchedConcurrent::handleEvent__(TransactionContext& context, const Event& event) -> void {
   bool hasWorkingAction = false;

   const auto total = getNumOfActions();
   SeqInt i = 0;
   for(; i < total; ++i) {
      if(!IS_CHILD_WORKING(i)) continue;

      ActionStatus status = get(i)->handleEvent(context, event);
      if(status.isWorking()) {
         hasWorkingAction = true;
      } else {
         children[i] = State::Done;
         if(status.isFailed()) reportFailure(status);
      }

      if(event.isConsumed()) break;
   }

   if(!hasWorkingAction && !hasWorkingChildren(i)) {
      state = State::Done;
   }
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::handleEvent_(TransactionContext& context, const Event& event) -> Status {
   handleEvent__(context, event);
   if (state == State::Working && ActionStatus(finalStatus).isFailed()) {
      cleanUp(context, finalStatus);
   }

   if(!IS_WORKING__(state)) return finalStatus;

   return event.isConsumed() ? Result::CONTINUE : Result::UNKNOWN_EVENT;
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
