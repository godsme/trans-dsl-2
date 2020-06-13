//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_TRANSACTIONCONTEXT_H
#define TRANS_DSL_2_TRANSACTIONCONTEXT_H

#include <trans-dsl/tsl_ns.h>
#include <cub/dci/Role.h>
#include <assert.h>
#include <trans-dsl/sched/concept/RuntimeContext.h>

TSL_NS_BEGIN

struct TransactionInfo;

struct RuntimeContextInfo {
   auto getRuntimeContext() const -> RuntimeContext& {
      assert(currentRuntimeContext != nullptr);
      return *currentRuntimeContext;
   }

   void setRuntimeContext(RuntimeContext& runtimeContext) {
      currentRuntimeContext = &runtimeContext;
   }

   void reportFailure(const Status status) {
      currentRuntimeContext->reportFailure(status);
   }

private:
   RuntimeContext* currentRuntimeContext = nullptr;
};

struct TransactionContext: RuntimeContextInfo {
   HAS_ROLE(TransactionInfo);
};

TSL_NS_END

#endif //TRANS_DSL_2_TRANSACTIONCONTEXT_H
