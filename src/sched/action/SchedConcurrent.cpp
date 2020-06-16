//
// Created by Darwin Yuan on 2020/6/13.
//

#include <trans-dsl/sched/action/SchedConcurrent.h>
#include <event/concept/Event.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/sched/domain/RuntimeContextAutoSwitch.h>

TSL_NS_BEGIN

namespace {
   inline auto stillWorking(SchedConcurrent::State state) -> bool {
      return (state == SchedConcurrent::State::Working || state == SchedConcurrent::State::Stopping);
   }
}

#define IS_CHILD_WORKING(i) stillWorking(children[i])

///////////////////////////////////////////////////////////////////////////////
inline auto SchedConcurrent::notWorking() -> bool {
   return !stillWorking(state);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::startUp(TransactionContext& context) -> Status {
   auto hasWorkingChildren = false;
   for(SeqInt i=0; i < total; i++) {
      auto action = get(i);
      if(action == nullptr) {
         return Result::FATAL_BUG;
      }

      ActionStatus status = action->exec(context);
      children[i] = status.isWorking() ? State::Working : State::Done;
      if(status.isWorking()) {
         hasWorkingChildren = true;
      } else if(status.isFailed()) {
         return status;
      }
   }

   return hasWorkingChildren ? Result::CONTINUE : Result::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::cleanUp(TransactionContext& context, Status cause) -> Status {
   enableSandboxMode();

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

   return stillWorking(state) ? Result::CONTINUE : finalStatus;
}

#define AUTO_SWITCH()  RuntimeContextAutoSwitch autoSwitch__{context, *this}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::exec(TransactionContext& context) -> Status {
   if(state != State::Idle) {
      return FATAL_BUG;
   }

   total = getNumOfActions();
   attachToParent(context);

   AUTO_SWITCH();
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
   for(SeqInt i = from; i<total; i++) {
      if(IS_CHILD_WORKING(i)) return true;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////
inline auto SchedConcurrent::hasReportedError() const -> bool {
   return state == State::Working && hasFailure();
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::handleEvent__(TransactionContext& context, const Event& event) -> void {
   bool hasWorkingAction = false;
   SeqInt i = 0;
   for(; i < total; ++i) {
      if(!IS_CHILD_WORKING(i)) continue;

      ActionStatus status = get(i)->handleEvent(context, event);
      if(status.isWorking()) {
         hasWorkingAction = true;
         if(hasReportedError()) {
            children[i] = State::Stopping;
            break;
         }
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
   if (state == State::Working && hasFailure()) {
      cleanUp(context, finalStatus);
   }

   if(notWorking()) return finalStatus;

   return event.isConsumed() ? Result::CONTINUE : Result::UNKNOWN_EVENT;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::handleEvent(TransactionContext& context, const Event& event) -> Status {
   if(notWorking()) return Result::FATAL_BUG;

   AUTO_SWITCH();
   return handleEvent_(context, event);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::stop(TransactionContext& context, Status cause) -> Status {
   switch (state) {
   case State::Working: {
      AUTO_SWITCH();
      return cleanUp(context, cause);
   }
   case State::Stopping:
      return Result::CONTINUE;
   default:
      return Result::FATAL_BUG;
   }
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::kill(TransactionContext& context, Status cause) -> void {
   if(notWorking()) return;

   AUTO_SWITCH();
   for(SeqInt i = 0; i<total; i++) {
      if(IS_CHILD_WORKING(i)) get(i)->kill(context, cause);
   }

   state = State::Done;
}

TSL_NS_END
