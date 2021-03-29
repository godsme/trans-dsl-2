//
// Created by Darwin Yuan on 2020/7/1.
//

#include <trans-dsl/sched/action/SchedJoin.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/tsl_config.h>
#include <trans-dsl/sched/domain/MultiThreadContext.h>
#include <trans-dsl/sched/domain/ThreadDoneMsg.h>
#include <iostream>

TSL_NS_BEGIN

///////////////////////////////////////////////////////////////////////////////
auto SchedJoin::exec(TransactionContext& context) -> Status {
   likely_branch
   if(auto mt = context.getMultiThreadContext(); likely(mt != nullptr)) {
      bitMap = getThreadBitMap();
      auto status =  mt->join(bitMap);
      if(status != Result::SUCCESS) {
         return status;
      }

      return bitMap.empty() ? Result::SUCCESS : Result::CONTINUE;
   }

   return Result::FATAL_BUG;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedJoin::handleEvent(TransactionContext&, Event const& event) -> Status {
   if(!event.matches(EV_ACTION_THREAD_DONE)) {
      return Result::UNKNOWN_EVENT;
   }

   auto msg = reinterpret_cast<const ThreadDoneMsg*>(event.getMsg());
   unlikely_branch
   if(unlikely(msg->who >= sizeof(bitMap) * 8)) {
      return Result::FATAL_BUG;
   }

   if(msg->who == 0) {
      return Result::SUCCESS;
   }

   if(likely(bitMap.isEnabled(msg->who))) {
      bitMap.clear(msg->who);
   }

   return bitMap.empty() ? Result::SUCCESS : Result::CONTINUE;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedJoin::stop(TransactionContext&, Status cause) -> Status {
   bitMap = 0;
   return cause;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedJoin::kill(TransactionContext&, Status) -> void {
   bitMap = 0;
}

TSL_NS_END