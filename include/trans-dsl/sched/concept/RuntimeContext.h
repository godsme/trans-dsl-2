//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_RUNTIMECONTEXT_H
#define TRANS_DSL_2_RUNTIMECONTEXT_H

#include <cub/dci/Role.h>
#include <trans-dsl/tsl_status.h>
#include <trans-dsl/utils/ActionStatus.h>

TSL_NS_BEGIN

struct RuntimeContext {
   RuntimeContext() = default;

   auto getStatus() const -> Status {
      return finalStatus;
   }

   auto  hasFailure() const -> bool {
      return finalStatus != Result::SUCCESS;
   }

   auto cleanUpFailure() -> void {
      finalStatus = Result::SUCCESS;
   }

   auto reportFailure(ActionStatus status) -> void {
      if(status.isFailed()) {
         finalStatus = status;

         if(!immune && parentEnv != nullptr) {
            parentEnv->reportFailure(status);
         }
      }
   }

   auto syncParentFailure() {
      if(parentEnv != nullptr) {
         finalStatus = parentEnv->getStatus();
      }
   }

protected:
   RuntimeContext* parentEnv = nullptr;
   Status finalStatus = Result::SUCCESS;
   bool immune = false;
};

TSL_NS_END

#endif //TRANS_DSL_2_RUNTIMECONTEXT_H
