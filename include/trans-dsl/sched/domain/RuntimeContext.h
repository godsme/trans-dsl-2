//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_RUNTIMECONTEXT_H
#define TRANS_DSL_2_RUNTIMECONTEXT_H

#include <trans-dsl/tsl_status.h>

TSL_NS_BEGIN

struct TransactionContext;

// 16 bytes
struct RuntimeContext {
   RuntimeContext() = default;
   RuntimeContext(bool sandbox) : sandbox(sandbox) {}

   auto getStatus() const -> Status {
      return finalStatus;
   }

   auto getStopCause() const -> Status {
      return finalStatus == Result::SUCCESS ? Result::FORCE_STOPPED : finalStatus;
   }

   auto  hasFailure() const -> bool {
      return finalStatus != Result::SUCCESS;
   }

   auto cleanUpFailure() -> void {
      finalStatus = Result::SUCCESS;
   }

   auto reportFailure(Status status) -> void;
   auto attachToParent(TransactionContext& context) -> Status;

   auto enableSandboxMode() {
      sandbox = true;
   }
protected:
   RuntimeContext* parentEnv = nullptr;
   Status finalStatus = Result::SUCCESS;
   bool sandbox = false;
};

TSL_NS_END

#endif //TRANS_DSL_2_RUNTIMECONTEXT_H
