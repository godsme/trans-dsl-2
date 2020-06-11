//
// Created by Darwin Yuan on 2020/6/10.
//

#ifndef TRANS_DSL_2_STUPIDTRANSACTIONCONTEXT_H
#define TRANS_DSL_2_STUPIDTRANSACTIONCONTEXT_H

#include <trans-dsl/sched/concept/TransactionContext.h>
#include <trans-dsl/action/TransactionInfo.h>
#include <trans-dsl/sched/concept/RuntimeContext.h>

struct StupidTransactionContext
   : private TSL_NS::RuntimeContext, TSL_NS::TransactionInfo, TSL_NS::TransactionContext {

   StupidTransactionContext() {
      TSL_NS::TransactionContext::setRuntimeContext(*this);
   }

   OVERRIDE(getInstanceId() const -> TSL_NS::InstanceId) {
      return 0;
   }

   OVERRIDE(getStatus() const -> TSL_NS::Status) {
      return TransactionContext::getRuntimeContext().getStatus();
   }
   OVERRIDE(getUserContext() const -> TSL_NS::Unknown*) {
      return nullptr;
   }

private:
   IMPL_ROLE(TransactionInfo);
};

#endif //TRANS_DSL_2_STUPIDTRANSACTIONCONTEXT_H
