//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_RUNTIMECONTEXT_H
#define TRANS_DSL_2_RUNTIMECONTEXT_H

#include <cub/dci/Role.h>
#include <trans-dsl/tsl_status.h>
#include <trans-dsl/utils/ActionStatus.h>

TSL_NS_BEGIN

struct TransactionContext;

// 16 bytes
struct RuntimeContext {
   RuntimeContext() = default;
   RuntimeContext(bool immune) : immune(immune) {}

   auto getStatus() const -> Status {
      return finalStatus;
   }

   auto  hasFailure() const -> bool {
      return finalStatus != Result::SUCCESS;
   }

   auto cleanUpFailure() -> void {
      finalStatus = Result::SUCCESS;
   }

   auto reportFailure(ActionStatus status) -> void;
   auto syncParentFailure() -> void;
   auto attachToParent(TransactionContext& context) -> Status;

protected:
   RuntimeContext* parentEnv = nullptr;
   Status finalStatus = Result::SUCCESS;
   bool immune = false;
};

TSL_NS_END

#endif //TRANS_DSL_2_RUNTIMECONTEXT_H
