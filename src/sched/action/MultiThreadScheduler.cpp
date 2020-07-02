//
// Created by Darwin Yuan on 2020/7/2.
//

#include <trans-dsl/sched/action/MultiThreadScheduler.h>
#include <trans-dsl/sched/domain/SchedAction.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/tsl_config.h>
#include <trans-dsl/utils/AssertionHelper.h>
#include <event/impl/ConsecutiveEventInfo.h>
#include <trans-dsl/sched/domain/ThreadDoneMsg.h>

TSL_NS_BEGIN

#define FOREACH_THREAD__(i, from) for(ThreadId i = from; i < limits; i++)
#define FOREACH_THREAD(i) FOREACH_THREAD__(i, 0)

#define THREAD_DONE(i) do {               \
   threads[i] = nullptr;                  \
   --alive;                               \
   if(i != 0) {                           \
      newDone |= (ThreadBitMap(1) << i);  \
   }                                      \
} while(0)

#define MAIN_TID 0

namespace
{
   struct AutoThreadSwitch {
      AutoThreadSwitch(ThreadId newThread, ThreadId& current)
         : original(current)
         , current(current) {
         current = newThread;
      }

      ~AutoThreadSwitch() {
         current = original;
      }

   private:
      ThreadId original;
      ThreadId& current;
   };
}

///////////////////////////////////////////////////////////////////////////////////////
#define SWITCH_TO(tid) AutoThreadSwitch autoSwitch(tid, currentTid);

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::exec(ThreadId tid, TransactionContext& context) -> Status {
   SWITCH_TO(tid);
   auto status = threads[tid]->exec(context);
   if(status != Result::CONTINUE) {
      THREAD_DONE(tid);
   }

   return status;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::start(TransactionContext& context, SchedAction& action) -> Status {
   BUG_CHECK(state == State::INIT);

   threads = getThreads();
   // user config
   limits = getMaxThreads();

   context.updateMultiThreadContext(*this);
   threads[MAIN_TID] = &action;
   ++alive;
   state = State::WORKING;
   currentTid = MAIN_TID;

   auto status = exec(MAIN_TID, context);
   if(likely(is_not_working_status(status))) {
      state = State::DONE;
   }

   return status;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::handleEvent_(ThreadId i, TransactionContext& context, Event const& event) -> Status {
   SWITCH_TO(i);
   auto status = threads[i]->handleEvent(context, event);
   if(is_not_working_status(status)) {
      THREAD_DONE(i);
   }

   return status;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::othersHandleEvent(TransactionContext& context, Event const& event) -> Status {
   FOREACH_THREAD__(i, 1) {
      if(threads[i] == nullptr) continue;

      auto status = handleEvent_(i, context, event);
      if(cub::is_failed_status(status)) {
         return stop_(context, status);
      }

      if(event.isConsumed()) {
         return Result::CONTINUE;
      }
   }

   return Result::UNKNOWN_EVENT;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::handleEventWorking(TransactionContext& context, Event const& event) -> Status {
   auto status = handleEvent_(MAIN_TID, context, event);
   if(unlikely(threads[MAIN_TID] == nullptr)) {
      return kill_(context, status);
   }

   if(likely(event.isConsumed() || alive == 1)) {
      return status;
   }

   return othersHandleEvent(context, event);
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::fetchADoneTid() const -> ThreadId {
   ThreadId tid = 1;
   for(ThreadBitMap map = 2; !(newDone & map); map <<= 1, tid++);
   return tid;
}

///////////////////////////////////////////////////////////////////////////////////////
#define DONE_MSG(tid) ev::ConsecutiveEventInfo(EV_ACTION_THREAD_DONE, ThreadDoneMsg{tid})
///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::scheduleEvent(TransactionContext& context, Event const& event) -> Status {
   auto status = handleEventWorking(context, event);
   while(newDone != 0 && is_working_status(status)) {
      ThreadId tid = 1;
      ThreadBitMap map = 2;
      for(; !(newDone & map); map <<= 1, tid++);
      status = broadcast(context, DONE_MSG(tid));
      newDone &= ~map;
   }

   return status;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::handleEvent(TransactionContext& context, Event const& event) -> Status {
   switch (state) {
      case State::WORKING:
         return scheduleEvent(context, event);
      default:
         return Result::FATAL_BUG;
   }
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::stop_(TransactionContext& context, Status cause) -> Status {
   if(state == State::STOPPING) return Result::CONTINUE;

   context.reportFailure(cause);

   FOREACH_THREAD(i) {
      if(threads[i] == nullptr) continue;
      SWITCH_TO(i);
      auto status = threads[i]->stop(context, cause);
      if(is_not_working_status(status)) {
         THREAD_DONE(i);
         if(alive == 0) {
            state = State::DONE;
            return status;
         }
         if(cub::is_failed_status(status)) {
            context.reportFailure(status);
         }
      }
   }

   state = State::STOPPING;

   return Result::CONTINUE;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::stop(TransactionContext& context, Status cause) -> Status {
   switch (state) {
      case State::WORKING: {
         Status status = stop_(context, cause);
         if(status == Result::SUCCESS) {
            return Result::FORCE_STOPPED;
         }
         return status;
      }
      case State::STOPPING:
         return Result::CONTINUE;
      default:
         return Result::FATAL_BUG;
   }
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::kill__(TransactionContext& context, Status cause) -> void {
   FOREACH_THREAD(i) {
      if(threads[i] == nullptr) continue;
      SWITCH_TO(i);
      threads[i]->kill(context, cause);
      THREAD_DONE(i);
      if(alive == 0) {
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::kill_(TransactionContext& context, Status cause) -> Status {
   if(alive > 0) kill__(context, cause);
   state = State::DONE;
   return cause;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::kill(TransactionContext& context, Status cause) -> void {
   switch (state) {
      case State::WORKING:
      case State::STOPPING:
         kill_(context, cause);
      default:
         break;
   }
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::broadcastToOthers(TransactionContext& context, Event const& event) -> Status {
   FOREACH_THREAD__(i, 1) {
      if(threads[i] == nullptr) continue;
      auto status = handleEvent_(i, context, event);
      if(cub::is_failed_status(status)) {
         if(auto result = stop_(context, status); result != Result::CONTINUE) {
            return result;
         }
      }
   }

   return Result::CONTINUE;
}
/////////////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::broadcast(TransactionContext& context, Event const& event) -> Status {
   auto status = handleEvent_(MAIN_TID, context, event);
   if(unlikely(threads[MAIN_TID] == nullptr)) {
      return kill_(context, status);
   }

   return broadcastToOthers(context, event);
}

/////////////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::join(ThreadBitMap& bitMap) -> Status {
   if(bitMap == 0) {
      if(currentTid != MAIN_TID) {
         return Result::USER_FATAL_BUG;
      }

      FOREACH_THREAD__(i, 1) {
         if(threads[i] != nullptr && i != currentTid) {
            bitMap |= (ThreadBitMap(1) << i);
         }
      }
   }
   return Result::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::startThread(TransactionContext& context, ThreadId tid) -> Status {
   BUG_CHECK(tid < limits);
   BUG_CHECK(threads[tid] == nullptr);

   threads[tid] = createThread(tid);
   if(threads[tid] == nullptr) {
      return Result::FATAL_BUG;
   }

   alive++;

   auto status = exec(tid, context);
   return status == Result::CONTINUE ? Result::SUCCESS : status;
}

TSL_NS_END