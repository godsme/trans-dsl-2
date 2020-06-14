//
// Created by Darwin Yuan on 2020/6/11.
//

#include <trans-dsl/sched/action/SchedOptional.h>

TSL_NS_BEGIN

struct TransactionContext;

auto SchedOptional::exec(TransactionContext& context) -> Status {
   if(isTrue(context)) {
      action = getAction();
      return action->exec(context);
   }

   action = nullptr;
   return Result::SUCCESS;
}

auto SchedOptional::handleEvent(TransactionContext& context, const Event& event) -> Status {
   return action == nullptr ? Result::FATAL_BUG : action->handleEvent(context, event);
}

auto SchedOptional::stop(TransactionContext& context, Status cause)  -> Status {
   return action == nullptr ? Result::FATAL_BUG : action->stop(context, cause);
}

auto SchedOptional::kill(TransactionContext& context) -> void {
   if(action != nullptr) {
      action->kill(context);
      action = nullptr;
   }
}

TSL_NS_END
