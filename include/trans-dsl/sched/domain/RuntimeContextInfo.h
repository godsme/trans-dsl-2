//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_RUNTIMECONTEXTINFO_H
#define TRANS_DSL_2_RUNTIMECONTEXTINFO_H

#include <trans-dsl/sched/domain/RuntimeContext.h>
#include <trans-dsl/utils/ActionStatus.h>

TSL_NS_BEGIN

struct RuntimeContextInfo {
   auto getRuntimeContext() const -> RuntimeContext& {
      return *currentRuntimeContext;
   }

   auto setRuntimeContext(RuntimeContext& runtimeContext) -> void {
      currentRuntimeContext = &runtimeContext;
   }

   auto reportFailure(Status status) -> void {
      currentRuntimeContext->reportFailure(status);
   }

   auto getRuntimeEnvStatus() const -> Status {
      return currentRuntimeContext->getStatus();
   }

   auto hasFailure() const -> bool {
      return cub::is_failed_status(getRuntimeEnvStatus());
   }

private:
   RuntimeContext  topContext;
   RuntimeContext* currentRuntimeContext = &topContext;
};

TSL_NS_END

#endif //TRANS_DSL_2_RUNTIMECONTEXTINFO_H
