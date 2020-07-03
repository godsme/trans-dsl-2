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
#include <iostream>

TSL_NS_BEGIN

#define FOREACH_THREAD__(i, from) for(ThreadId i = from; i < limits; i++)
#define FOREACH_THREAD(i) FOREACH_THREAD__(i, 0)

#define THREAD_DONE(i) do {               \
   context.reportFailure(status);         \
   threads[i] = nullptr;                  \
   --alive;                               \
   if(i != 0) {                           \
      newDone |= ThreadBitMap(1 << i);    \
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
   } else if(context.getRuntimeContext().hasFailure()) {
      stopOthers(context, context.getRuntimeEnvStatus());
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
#define CHECK_FAILURE() do { \
   if(state == State::WORKING && context.hasFailure()) \
      if(auto status = stop_(context, context.getRuntimeEnvStatus()); is_not_working_status(status)) \
         return status; \
} while(0)

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::othersHandleEvent(TransactionContext& context, Event const& event) -> Status {
   FOREACH_THREAD__(i, 1) {
      if(threads[i] == nullptr) continue;
      (void) handleEvent_(i, context, event);
      CHECK_FAILURE();

      if(event.isConsumed()) {
         return Result::CONTINUE;
      }

      if(alive == 1) {
         break;
      }
   }

   return Result::UNKNOWN_EVENT;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::handleEventWorking(TransactionContext& context, Event const& event) -> Status {
   auto status = handleEvent_(MAIN_TID, context, event);
   if(is_not_working_status(status)) {
      cleanup(context, status);
      return status;
   }

   if(likely(event.isConsumed() || alive == 1)) {
      return status;
   }

   return othersHandleEvent(context, event);
}

///////////////////////////////////////////////////////////////////////////////////////
#define DONE_MSG(tid) ev::ConsecutiveEventInfo(EV_ACTION_THREAD_DONE, ThreadDoneMsg{tid})
///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::scheduleEvent(TransactionContext& context, Event const& event) -> Status {
   auto status = handleEventWorking(context, event);
   if(is_working_status(status)) {
      auto result = notifyDoneThreads(context);
      if(is_not_working_status(result)) {
         return result;
      }
   }

   return status;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::handleEvent(TransactionContext& context, Event const& event) -> Status {
   switch (state) {
      case State::WORKING:
      case State::STOPPING:
         return scheduleEvent(context, event);
      default:
         return Result::FATAL_BUG;
   }
}

auto MultiThreadScheduler::stop__(ThreadId tid, TransactionContext& context, Status cause) -> Status {
   SWITCH_TO(tid);
   auto status = threads[tid]->stop(context, cause);
   if(is_not_working_status(status)) {
      THREAD_DONE(tid);
   }

   return status;
}

auto MultiThreadScheduler::cleanup(TransactionContext& context, Status cause) -> void {
   kill(context, cause);
   state = State::DONE;
}

auto MultiThreadScheduler::stopOthers(TransactionContext& context, Status cause) -> void {
   if(alive > 1) {
      FOREACH_THREAD__(i, 1) {
         if(threads[i] == nullptr) continue;
         (void) stop__(i, context, cause);
         if(alive == 1) break;;
      }
   }

   state = State::STOPPING;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::stop_(TransactionContext& context, Status cause) -> Status {
   if(state == State::STOPPING) return Result::CONTINUE;

   auto status = stop__(MAIN_TID, context, cause);
   if(is_not_working_status(status)) {
      cleanup(context, cause);
      return status;
   }

   stopOthers(context, cause);

   return Result::CONTINUE;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::notifyDoneThreads(TransactionContext& context) -> Status {
   while(newDone != 0) {
      ThreadId tid = 1;
      ThreadBitMap map = 2;
      for(; !(newDone & map); map <<= 1, tid++);
      auto status = broadcast(context, DONE_MSG(tid));
      if(is_not_working_status(status)) {
         return status;
      }
      newDone &= ThreadBitMap(~map);
   }

   return Result::CONTINUE;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::stop(TransactionContext& context, Status cause) -> Status {
   switch (state) {
      case State::WORKING: {
         auto status = stop_(context, cause);
         if(is_not_working_status(status)) {
            return status;
         }
         return notifyDoneThreads(context);
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
      threads[i] = nullptr;
      if(--alive == 0) {
         break;
      }
   }
   state = State::DONE;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::kill_(TransactionContext& context, Status cause) -> void {
   if(alive > 0) kill__(context, cause);
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
      (void) handleEvent_(i, context, event);
      CHECK_FAILURE();
      if(alive == 1) break;
   }

   return Result::CONTINUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::broadcast(TransactionContext& context, Event const& event) -> Status {
   auto status = handleEvent_(MAIN_TID, context, event);
   if(unlikely(is_not_working_status(status))) {
      cleanup(context, status);
      return status;
   }

   if(alive == 1) {
      return Result::CONTINUE;
   }

   return broadcastToOthers(context, event);
}

/////////////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::joinAll(ThreadBitMap& bitMap) -> Status {
   if(currentTid != MAIN_TID) {
      return Result::USER_FATAL_BUG;
   }

   FOREACH_THREAD__(i, 1) {
      if(threads[i] != nullptr && i != currentTid) {
         bitMap |= ThreadBitMap(1 << i);
      }
   }

   return Result::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::join(ThreadBitMap& bitMap) -> Status {
   if(bitMap == 0) {
      return joinAll(bitMap);
   }

   // clear impossible threads.
   bitMap &= ThreadBitMap((1 << limits) - 1);

   // clear non-exist (yet) threads & self
   FOREACH_THREAD(i) {
      if(threads[i] == nullptr || i == currentTid) {
         bitMap &= ~ThreadBitMap(1 << i);
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
   if(is_working_status(status)) {
      CHECK_FAILURE();
      return Result::SUCCESS;
   }

   return status;
}

TSL_NS_END