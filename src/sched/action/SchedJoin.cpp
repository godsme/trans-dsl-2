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
      bitMap = mt->join(getThreadBitMap());
      return bitMap == 0 ? Result::SUCCESS : Result::CONTINUE;
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

   auto mask = ThreadBitMap(1) << msg->who;
   if(likely(bitMap & mask)) {
      bitMap &= ~mask;
   }

   return bitMap == 0 ? Result::SUCCESS : Result::CONTINUE;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedJoin::stop(TransactionContext&, Status) -> Status {
   bitMap = 0;
   return Result::FORCE_STOPPED;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedJoin::kill(TransactionContext&, Status) -> void {
   bitMap = 0;
}

TSL_NS_END