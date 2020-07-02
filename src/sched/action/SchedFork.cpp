//
// Created by Darwin Yuan on 2020/7/1.
//

#include <trans-dsl/sched/action/SchedFork.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/tsl_config.h>
#include <trans-dsl/sched/domain/MultiThreadContext.h>

TSL_NS_BEGIN

auto SchedFork::exec(TransactionContext& context) -> Status {
   likely_branch
   if(auto mt = context.getMultiThreadContext(); likely(mt != nullptr)) {
      return mt->startThread(context, getThreadId());
   }

   return Result::FATAL_BUG;
}

TSL_NS_END