//
// Created by Darwin Yuan on 2020/7/2.
//

#include <trans-dsl/sched/action/SchedMultiThread.h>
#include <trans-dsl/sched/domain/SchedAction.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/tsl_config.h>
#include <trans-dsl/utils/AssertionHelper.h>
#include <event/impl/ConsecutiveEventInfo.h>
#include <trans-dsl/sched/domain/ThreadDoneMsg.h>

TSL_NS_BEGIN

///////////////////////////////////////////////////////////////////////////////////////
#define FOREACH_THREAD__(i, from) for(ThreadId i = from; i < limits; i++)
#define FOREACH_THREAD(i) FOREACH_THREAD__(i, 0)

///////////////////////////////////////////////////////////////////////////////////////
#define THREAD_DONE(i) do {                  \
   context.reportFailure(status);            \
   threads[i] = nullptr;                     \
   --alive;                                  \
   if(i != 0 ) {                             \
      newDone.enable(i);                     \
   }                                         \
} while(0)

///////////////////////////////////////////////////////////////////////////////////////
#define MAIN_TID 0

///////////////////////////////////////////////////////////////////////////////////////
#define DONE_MSG(tid) ev::ConsecutiveEventInfo{EV_ACTION_THREAD_DONE, ThreadDoneMsg{tid}}

///////////////////////////////////////////////////////////////////////////////////////
#define SCHED_THREAD(tid, expr) do { \
   SWITCH_TO(tid); \
   auto status = threads[tid]->expr; \
   if(unlikely(!(Result::__WORKING_STATUS_BEGIN & status))) \
      THREAD_DONE(tid); \
   return status;   \
} while(0)

///////////////////////////////////////////////////////////////////////////////////////
#define MAIN_THREAD_DONE_CHECK(status) do { \
   if(unlikely(!(Result::__WORKING_STATUS_BEGIN & status))) { \
       kill_(context, status);       \
       return status;                \
   } \
}while(0)

///////////////////////////////////////////////////////////////////////////////////////
#define EXPECT_CONTINUE(expr) do { \
   if(auto __result = expr; unlikely(!(Result::__WORKING_STATUS_BEGIN & __result))) { \
        return __result; \
   } \
} while(0)

///////////////////////////////////////////////////////////////////////////////////////
#define THREAD_EXIST_CHECK(tid) if(threads[tid] == nullptr) continue

///////////////////////////////////////////////////////////////////////////////////////
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
auto SchedMultiThread::exec_(ThreadId tid, TransactionContext& context) -> Status {
   SCHED_THREAD(tid, exec(context));
}

///////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::checkJoinAll(TransactionContext& context) -> Status {
   if(alive > 1 || newDone.empty()  || joinBitMaps[MAIN_TID].empty()) {
      return Result::CONTINUE;
   }

   newDone = 0;
   joinBitMaps[MAIN_TID] = 0;

   auto status = handleEvent_(MAIN_TID, context, DONE_MSG(MAIN_TID));
   MAIN_THREAD_DONE_CHECK(status);

   return status;
}

///////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::start(TransactionContext& context, SchedAction& action) -> Status {
   BUG_CHECK(state == State::INIT);

   context.updateMultiThreadContext(*this);

   threads = getThreads();
   limits  = getMaxThreads();

   threads[MAIN_TID] = &action;
   ++alive;
   state = State::WORKING;
   currentTid = MAIN_TID;

   auto status = exec_(MAIN_TID, context);
   unlikely_branch
   if(unlikely(is_not_working_status(status))) {
      state = State::DONE;
   } else unlikely_branch
   if(unlikely(context.hasFailure())) {
      stopOthers(context, context.getRuntimeEnvStatus());
      return checkJoinAll(context);
   }

   return status;
}

///////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::handleEvent_(
   ThreadId tid, TransactionContext& context, Event const& event) -> Status {
   SCHED_THREAD(tid, handleEvent(context, event));
}

///////////////////////////////////////////////////////////////////////////////////////
#define CHECK_FAILURE() do { \
   unlikely_branch           \
   if(unlikely(state == State::WORKING && context.hasFailure())) \
      EXPECT_CONTINUE(stopAll(context, context.getRuntimeEnvStatus())); \
} while(0)

///////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::othersHandleEvent(TransactionContext& context, Event const& event) -> Status {
   FOREACH_THREAD__(i, 1) {
      THREAD_EXIST_CHECK(i);
      (void) handleEvent_(i, context, event);
      CHECK_FAILURE();

      if(event.isConsumed()) return Result::CONTINUE;
      if(alive == 1) break;
   }

   return Result::UNKNOWN_EVENT;
}

///////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::handleEventWorking(TransactionContext& context, Event const& event) -> Status {
   auto status = handleEvent_(MAIN_TID, context, event);
   MAIN_THREAD_DONE_CHECK(status);

   if(likely(event.isConsumed() || alive == 1)) {
      return status;
   }

   return othersHandleEvent(context, event);
}


///////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::scheduleEvent(TransactionContext& context, Event const& event) -> Status {
   auto status = handleEventWorking(context, event);
   EXPECT_CONTINUE(status);

   unlikely_branch
   if(unlikely(newDone.nonEmpty())) {
      EXPECT_CONTINUE(notifyDoneThreads(context));
   }

   return status;
}

///////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::handleEvent(TransactionContext& context, Event const& event) -> Status {
   switch (state) {
      likely_branch
      case State::WORKING: [[fallthrough]];
      likely_branch
      case State::STOPPING:
         return scheduleEvent(context, event);
      default:
         return Result::FATAL_BUG;
   }
}

//////////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::stop_(ThreadId tid, TransactionContext& context, Status cause) -> Status {
   SCHED_THREAD(tid, stop(context, cause));
}

//////////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::stopOthers(TransactionContext& context, Status cause) -> void {
   if(alive > 1) {
      FOREACH_THREAD__(i, 1) {
         THREAD_EXIST_CHECK(i);
         (void) stop_(i, context, cause);
         if(alive == 1) break;;
      }
   }

   state = State::STOPPING;
}

///////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::stopAll(TransactionContext& context, Status cause) -> Status {
   if(state == State::STOPPING) return Result::CONTINUE;

   auto status = stop_(MAIN_TID, context, cause);
   MAIN_THREAD_DONE_CHECK(status);

   stopOthers(context, cause);

   return Result::CONTINUE;
}

///////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::notifyDoneThreads(TransactionContext& context) -> Status {
   EXPECT_CONTINUE(checkJoinAll(context));

   while(newDone.nonEmpty()) {
      ThreadId tid = newDone.firstEnabled();
      EXPECT_CONTINUE(broadcast(context, joinBitMaps[tid], DONE_MSG(tid)));
      joinBitMaps[tid] = 0;
      newDone.clear(tid);

      EXPECT_CONTINUE(checkJoinAll(context));
   }

   return Result::CONTINUE;
}

///////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::stop(TransactionContext& context, Status cause) -> Status {
   switch (state) {
      case State::WORKING: {
         EXPECT_CONTINUE(stopAll(context, cause));
         return notifyDoneThreads(context);
      }
      case State::STOPPING:
         return Result::CONTINUE;
      default:
         return Result::FATAL_BUG;
   }
}

///////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::kill_(TransactionContext& context, Status cause) -> void {
   likely_branch
   if(likely(alive > 0)) {
      FOREACH_THREAD(i) {
         THREAD_EXIST_CHECK(i);
         SWITCH_TO(i);
         threads[i]->kill(context, cause);
         threads[i] = nullptr;
         if(--alive == 0) {
            break;
         }
      }
   }

   state = State::DONE;
}

///////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::kill(TransactionContext& context, Status cause) -> void {
   switch (state) {
      case State::WORKING:
      case State::STOPPING:
         kill_(context, cause);
      default:
         break;
   }
}

///////////////////////////////////////////////////////////////////////////////////////
#define JOIN_ENSURE(tid) \
   if(!joinTable.isEnabled(i)) continue; \
   joinTable.clear(i)

///////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::broadcastToOthers(TransactionContext& context, ThreadBitMap& joinTable, Event const& event) -> Status {
   FOREACH_THREAD__(i, 1) {
      JOIN_ENSURE(i);
      THREAD_EXIST_CHECK(i);

      (void) handleEvent_(i, context, event);
      CHECK_FAILURE();

      if(joinTable.empty() || alive == 1) break;
   }

   return Result::CONTINUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::broadcast(TransactionContext& context, ThreadBitMap& joinTable, Event const& event) -> Status {
   if(joinTable.isEnabled(MAIN_TID)) {
      joinTable.clear(MAIN_TID);
      auto status = handleEvent_(MAIN_TID, context, event);
      MAIN_THREAD_DONE_CHECK(status);
   }

   if(alive == 1 || joinTable.empty()) {
      return Result::CONTINUE;
   }

   return broadcastToOthers(context, joinTable, event);
}

/////////////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::joinAll(ThreadBitMap& bitMap) -> Status {
   if(currentTid != MAIN_TID) {
      return Result::USER_FATAL_BUG;
   }

   if(alive > 1) {
      joinBitMaps[MAIN_TID] = 1;
      bitMap = 1;
   }

   return Result::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::join(ThreadBitMap& bitMap) -> Status {
   if(bitMap.empty()) {
      return joinAll(bitMap);
   }

   // clear impossible threads.
   bitMap.resize(limits);

   // clear non-exist (yet) threads & self
   FOREACH_THREAD(i) {
      if((forkedBitMap.isEnabled(i) && threads[i] == nullptr) || i == currentTid) {
         bitMap.clear(i);
      } else if(bitMap.isEnabled(i)) {
         joinBitMaps[i].enable(currentTid);
         if(currentTid == MAIN_TID) {
            joinBitMaps[MAIN_TID].enable(i);
         }
      }
   }

   return Result::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////
auto SchedMultiThread::startThread(TransactionContext& context, ThreadId tid) -> Status {
   BUG_CHECK(tid < limits);
   BUG_CHECK(!forkedBitMap.isEnabled(tid));
   BUG_CHECK(threads[tid] == nullptr);

   forkedBitMap.enable(tid);

   threads[tid] = createThread(tid);
   if(threads[tid] == nullptr) {
      return Result::FATAL_BUG;
   }

   alive++;

   auto status = exec_(tid, context);
   if(is_working_status(status)) {
      //CHECK_FAILURE();
      return Result::SUCCESS;
   }

   return status;
}

TSL_NS_END