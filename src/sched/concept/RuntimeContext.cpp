//
// Created by Darwin Yuan on 2020/6/12.
//
#include <trans-dsl/sched/concept/RuntimeContext.h>
#include <trans-dsl/sched/concept/TransactionContext.h>

TSL_NS_BEGIN

namespace {
   bool shouldUpdate(ActionStatus current, ActionStatus newStatus) {
      if(current == newStatus || !newStatus.isFailed()) return false;
      return (newStatus == Result::FORCE_STOPPED) ? current == Result::SUCCESS : true;
   }
}

auto RuntimeContext::reportFailure(ActionStatus status) -> void {
   if(shouldUpdate(finalStatus, status)) {
      finalStatus = status;

      if(!sandbox && parentEnv != nullptr) {
         parentEnv->reportFailure(status);
      }

      sandbox = true;
   }
}

auto RuntimeContext::attachToParent(TransactionContext& context) -> Status {
   if(parentEnv == nullptr) {
      parentEnv = &context.getRuntimeContext();
      return Result::SUCCESS;
   }

   return Result::FATAL_BUG;
}

TSL_NS_END