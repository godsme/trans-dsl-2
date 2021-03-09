//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_TRANSACTIONCONTEXT_H
#define TRANS_DSL_2_TRANSACTIONCONTEXT_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/domain/RuntimeContext.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/action/TransactionInfo.h>
#include <trans-dsl/sched/domain/RuntimeContextInfo.h>
#include <trans-dsl/sched/domain/TransactionListener.h>

TSL_NS_BEGIN

struct TimerInfo;
struct MultiThreadContext;

struct TransactionContext
   : TransactionInfo
   , RuntimeContextInfo {

   auto getTimerInfo() const -> TimerInfo* {
      return timerInfo;
   }

   auto getListener() const -> TransactionListener& {
      return *listener;
   }

   auto getMultiThreadContext() const -> MultiThreadContext* {
      return multiThreadContext;
   }

   auto updateMultiThreadContext(MultiThreadContext& multiThreadContext) -> void {
      this->multiThreadContext = &multiThreadContext;
   }

private:
   OVERRIDE(getStatus() const -> Status) {
      return RuntimeContextInfo::getRuntimeEnvStatus();
   }

protected:
   using TransactionInfo::updateInstanceId;
   using TransactionInfo::updateUserContext;

   auto updateTimerInfo(TimerInfo& info) -> void {
      timerInfo = &info;
   }

   auto updateListener(TransactionListener& listener) -> void {
      this->listener = &listener;
   }

private:
   static TransactionListener& getNullListener() {
      static TransactionListener instance;
      return instance;
   }

private:
   TimerInfo* timerInfo = nullptr;
   TransactionListener* listener = &getNullListener();
   MultiThreadContext* multiThreadContext = nullptr;
};

TSL_NS_END

#endif //TRANS_DSL_2_TRANSACTIONCONTEXT_H
