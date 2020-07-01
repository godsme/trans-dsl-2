//
// Created by Darwin Yuan on 2020/6/11.
//

#include <trans-dsl/sched/action/SchedOptional.h>
#include <trans-dsl/utils/AssertionHelper.h>

TSL_NS_BEGIN

struct TransactionContext;

auto SchedOptional::exec(TransactionContext& context) -> Status {
   BUG_CHECK(action == nullptr);

   if(!isTrue(context)) {
      return Result::SUCCESS;
   }

   action = getAction();
   return action->exec(context);
}

auto SchedOptional::handleEvent(TransactionContext& context, Event const& event) -> Status {
   BUG_CHECK(action != nullptr);
   return action->handleEvent(context, event);
}

auto SchedOptional::stop(TransactionContext& context, Status cause)  -> Status {
   BUG_CHECK(action != nullptr);
   return action->stop(context, cause);
}

auto SchedOptional::kill(TransactionContext& context, Status cause) -> void {
   if(action != nullptr) {
      action->kill(context, cause);
      action = nullptr;
   }
}

TSL_NS_END
