//
// Created by Darwin Yuan on 2020/7/7.
//

#include <trans-dsl/sched/action/SchedWithId.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/utils/AssertionHelper.h>

TSL_NS_BEGIN

///////////////////////////////////////////////////////////////////////////////
auto SchedWithId::exec(TransactionContext& context) -> Status {
   BUG_CHECK(state == State::INIT);
   actionId = getActionId();
   state = State::WORKING;

   context.getListener().onActionStarting(actionId, context);
   auto status = ROLE(SchedAction).exec(context);
   if(status != Result::CONTINUE) {
      context.getListener().onActionDone(actionId, context, status);
      state = State::DONE;
   }

   return status;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedWithId::handleEvent(TransactionContext& context, Event const& event) -> Status {
   switch (state) {
      case State::WORKING:
      case State::STOPPING: {
         auto status = ROLE(SchedAction).handleEvent(context, event);
         if(status != Result::UNKNOWN_EVENT) {
            context.getListener().onActionEventConsumed(actionId, context, event);
            if(status != Result::CONTINUE) {
               context.getListener().onActionDone(actionId, context, status);
               state = State::DONE;
            }
         }
         return status;
      }
      default:
         return FATAL_BUG;
   }
}

///////////////////////////////////////////////////////////////////////////////
auto SchedWithId::stop(TransactionContext& context, Status cause) -> Status {
   switch (state) {
      case State::STOPPING:
         return Result::CONTINUE;
      case State::WORKING: {
         context.getListener().onActionStartStopping(actionId, context, cause);
         auto status = ROLE(SchedAction).stop(context, cause);
         if (is_not_working_status(status)) {
            state = State::DONE;
            context.getListener().onActionStopped(actionId, context, status);
         } else {
            state = State::STOPPING;
         }
      }
      default:
         return FATAL_BUG;
   }
}

///////////////////////////////////////////////////////////////////////////////
auto SchedWithId::kill(TransactionContext& context, Status cause) -> void {
   switch (state) {
      case State::STOPPING:
      case State::WORKING: {
         ROLE(SchedAction).kill(context, cause);
         context.getListener().onActionKilled(actionId, context, cause);
         state = State::DONE;
      }
      default: break;
   }
}


TSL_NS_END