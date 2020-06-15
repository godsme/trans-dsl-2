//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_TRANSACTIONCONTEXT_H
#define TRANS_DSL_2_TRANSACTIONCONTEXT_H

#include <trans-dsl/tsl_ns.h>
#include <cub/dci/Role.h>
#include <assert.h>
#include <trans-dsl/sched/concept/RuntimeContext.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/action/TransactionInfo.h>
#include "RuntimeContextInfo.h"

TSL_NS_BEGIN

struct TransactionContext
   : TransactionInfo
   , RuntimeContextInfo {

private:
   OVERRIDE(getStatus() const -> Status) {
      return RuntimeContextInfo::getRuntimeEnvStatus();
   }

protected:
   using TransactionInfo::updateInstanceId;
   using TransactionInfo::updateUserContext;
};

TSL_NS_END

#endif //TRANS_DSL_2_TRANSACTIONCONTEXT_H
