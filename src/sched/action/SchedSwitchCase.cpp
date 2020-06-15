//
// Created by Darwin Yuan on 2020/6/15.
//

#include <trans-dsl/sched/action/SchedSwitchCase.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/concept/TransactionContext.h>
#include <trans-dsl/sched/action/ActionPath.h>

TSL_NS_BEGIN

///////////////////////////////////////////////////////////////////////////////
auto SchedSwitchCase::exec(TransactionContext& context) -> Status {
   if(action != nullptr) return Result::FATAL_BUG;

   for(auto path = getNext(); path != nullptr; path = getNext()) {
      if(path->shouldExecute(context.ROLE(TransactionInfo))) {
         action = &path->getAction();
         return action->exec(context);
      }
   }

   return Result::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSwitchCase::handleEvent(TransactionContext& context, const Event& event) -> Status {
   if(action == nullptr) return FATAL_BUG;
   return action->handleEvent(context, event);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSwitchCase::stop(TransactionContext& context, Status cause) -> Status {
   if(action == nullptr) return Result::FATAL_BUG;
   return action->stop(context, cause);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSwitchCase::kill(TransactionContext& context, Status cause) -> void {
   if(action != nullptr) {
      action->kill(context, cause);
   }
}


TSL_NS_END