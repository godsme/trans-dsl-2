//
// Created by Darwin Yuan on 2020/6/13.
//

#include <trans-dsl/sched/action/SchedConcurrent.h>
#include <event/concept/Event.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/sched/domain/RuntimeContextAutoSwitch.h>
#include <trans-dsl/tsl_config.h>

TSL_NS_BEGIN

namespace {
   inline auto stillWorking(SchedConcurrent::State state) -> bool {
       switch (state) {
          case SchedConcurrent::State::Working:
          case SchedConcurrent::State::Stopping:
             return true;
          default:
             return false;
       }
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

      unlikely_branch
      if(unlikely(action == nullptr)) {
         return Result::FATAL_BUG;
      }

      Status status = action->exec(context);
      likely_branch
      if(likely(status == Result::CONTINUE)) {
         hasWorkingChildren = true;
         children[i] = State::Working;
      } else {
         children[i] = State::Done;
         unlikely_branch
         if(unlikely(cub::is_failed_status(status))) {
            return status;
         }
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

      Status status = get(i)->stop(context, cause);
      unlikely_branch
      if(unlikely(status == Result::CONTINUE)) {
         hasWorking = true;
         children[i] = State::Stopping;
      } else {
         children[i] = State::Done;
         likely_branch
         if(likely(cub::is_failed_status(status))) {
            reportFailure(status);
         }
      }
   }

   state = hasWorking ? State::Stopping : State::Done;

   return hasWorking ? Result::CONTINUE : finalStatus;
}

#define AUTO_SWITCH()  RuntimeContextAutoSwitch autoSwitch__{context, *this}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::exec(TransactionContext& context) -> Status {
   unlikely_branch
   if(unlikely(state != State::Idle)) {
      return FATAL_BUG;
   }

   total = getNumOfActions();
   attachToParent(context);

   AUTO_SWITCH();
   Status status = startUp(context);
   unlikely_branch
   if(unlikely(cub::is_failed_status(status))) {
      reportFailure(status);
      return cleanUp(context, finalStatus);
   }

   state = (status == Result::CONTINUE) ? State::Working : State::Done;

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
auto SchedConcurrent::handleEvent__(TransactionContext& context, Event const& event) -> void {
   bool hasWorkingAction = false;
   SeqInt i = 0;
   for(; i < total; ++i) {
      if(!IS_CHILD_WORKING(i)) continue;

      Status status = get(i)->handleEvent(context, event);
      if(Result::__WORKING_STATUS_BEGIN & status) {
         hasWorkingAction = true;

         unlikely_branch
         if(unlikely(status == Result::CONTINUE && hasReportedError())) {
            children[i] = State::Stopping;
            break;
         }
      } else {
         children[i] = State::Done;

         unlikely_branch
         if(unlikely(cub::is_failed_status(status))) {
            reportFailure(status);
            break;
         }
      }

      if(event.isConsumed()) break;
   }

   unlikely_branch
   if(unlikely(!hasWorkingAction && !hasWorkingChildren(i))) {
      state = State::Done;
   }
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::handleEvent_(TransactionContext& context, const Event& event) -> Status {
   handleEvent__(context, event);
   unlikely_branch
   if (unlikely(state == State::Working && hasFailure())) {
      cleanUp(context, finalStatus);
   }

   if(unlikely(notWorking())) return finalStatus;

   return event.isConsumed() ? Result::CONTINUE : Result::UNKNOWN_EVENT;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::handleEvent(TransactionContext& context, Event const& event) -> Status {
   unlikely_branch
   if(unlikely(notWorking())) return Result::FATAL_BUG;

   AUTO_SWITCH();
   return handleEvent_(context, event);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::stop(TransactionContext& context, Status cause) -> Status {
   switch (state) {
   likely_branch
   case State::Working: {
      AUTO_SWITCH();
      return cleanUp(context, cause);
   }
   case State::Stopping:
      return Result::CONTINUE;

   unlikely_branch
   default:
      return Result::FATAL_BUG;
   }
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::kill(TransactionContext& context, Status cause) -> void {
   unlikely_branch
   if(unlikely(notWorking())) return;

   AUTO_SWITCH();
   for(SeqInt i = 0; i<total; i++) {
      if(IS_CHILD_WORKING(i)) get(i)->kill(context, cause);
   }

   state = State::Done;
}

TSL_NS_END
