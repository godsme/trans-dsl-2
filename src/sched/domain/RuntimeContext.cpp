//
// Created by Darwin Yuan on 2020/6/12.
//
#include <trans-dsl/sched/domain/RuntimeContext.h>
#include <trans-dsl/sched/domain/TransactionContext.h>

TSL_NS_BEGIN

namespace {
   bool shouldUpdate(Status current, Status newStatus) {
      return (current != newStatus && cub::is_failed_status(newStatus));
   }
}

auto RuntimeContext::reportFailure(Status status) -> void {
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