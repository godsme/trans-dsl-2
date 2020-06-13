//
// Created by Darwin Yuan on 2020/6/12.
//
#include <trans-dsl/sched/concept/RuntimeContext.h>
#include <trans-dsl/sched/concept/TransactionContext.h>

TSL_NS_BEGIN

auto RuntimeContext::reportFailure(ActionStatus status) -> void {
   if(status.isFailed()) {
      finalStatus = status;

      if(!immune && parentEnv != nullptr) {
         parentEnv->reportFailure(status);
      }
   }
}

auto RuntimeContext::syncParentFailure() -> void {
   assert(parentEnv != nullptr);
   if(parentEnv != nullptr) {
      finalStatus = parentEnv->getStatus();
   }

   if(finalStatus == Result::SUCCESS) {
      finalStatus = Result::FORCE_STOPPED;
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