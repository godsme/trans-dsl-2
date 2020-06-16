//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_TRANSACTIONCONTEXT_H
#define TRANS_DSL_2_TRANSACTIONCONTEXT_H

#include <trans-dsl/tsl_ns.h>
#include <cub/dci/Role.h>
#include <assert.h>
#include <trans-dsl/sched/domain/RuntimeContext.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/action/TransactionInfo.h>
#include "RuntimeContextInfo.h"

TSL_NS_BEGIN

struct TimerInfo;
struct TransactionListener;

struct TransactionContext
   : TransactionInfo
   , RuntimeContextInfo {

   auto getTimerInfo() const -> TimerInfo* {
      return timerInfo;
   }

   auto getListener() const -> TransactionListener* {
      return listener;
   }

private:
   OVERRIDE(getStatus() const -> Status) {
      return RuntimeContextInfo::getRuntimeEnvStatus();
   }

protected:
   using TransactionInfo::updateInstanceId;
   using TransactionInfo::updateUserContext;

   auto updateTimerInfo(TimerInfo* info) -> void {
      timerInfo = info;
   }

   auto updateListener(TransactionListener* listener) -> void {
      this->listener = listener;
   }
private:
   TimerInfo* timerInfo = nullptr;
   TransactionListener* listener = nullptr;
};

TSL_NS_END

#endif //TRANS_DSL_2_TRANSACTIONCONTEXT_H
