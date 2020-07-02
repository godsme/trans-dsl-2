//
// Created by Darwin Yuan on 2020/7/2.
//

#include <trans-dsl/sched/action/MultiThreadScheduler.h>
#include <event/concept/Event.h>
#include <trans-dsl/sched/domain/SchedAction.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/tsl_config.h>
#include <trans-dsl/utils/AssertionHelper.h>

TSL_NS_BEGIN

#define FOREACH_THREAD__(i, from) for(ThreadId i = from; i < limits; i++)
#define FOREACH_THREAD(i) FOREACH_THREAD__(i, 0)

#define THREAD_DONE(i) threads[i] = nullptr; --alive
#define MAIN_TID 0

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::start(TransactionContext& context, SchedAction& action) -> Status {
   BUG_CHECK(state == State::INIT);

   // user config
   limits = MAX_NUM_OF_THREADS;

   context.updateMultiThreadContext(*this);
   threads[MAIN_TID] = &action;
   ++alive;
   state = State::WORKING;

   auto status = action.exec(context);
   if(likely(is_not_working_status(status))) {
      THREAD_DONE(0);
      state = State::DONE;
   }

   return status;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::handleEvent_(ThreadId i, TransactionContext& context, Event const& event) -> Status {
   auto status = threads[i]->handleEvent(context, event);
   if(is_not_working_status(status)) {
      THREAD_DONE(i);
   }
   return status;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::stop_(TransactionContext& context, Status cause) -> Status {
   if(state == State::STOPPING) return Result::CONTINUE;

   context.reportFailure(cause);

   FOREACH_THREAD(i) {
      if(threads[i] == nullptr) continue;
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
auto MultiThreadScheduler::kill__(TransactionContext& context, Status cause) -> void {
   FOREACH_THREAD(i) {
      if(threads[i] == nullptr) continue;
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
auto MultiThreadScheduler::handleEvent(TransactionContext& context, Event const& event) -> Status {
   switch (state) {
      case State::WORKING:
         return handleEventWorking(context, event);
      default:
         return Result::FATAL_BUG;
   }
}

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

auto MultiThreadScheduler::kill(TransactionContext& context, Status cause) -> void {
   switch (state) {
      case State::WORKING:
      case State::STOPPING:
         kill_(context, cause);
      default:
         break;
   }
}

auto MultiThreadScheduler::join(ThreadBitMap) -> ThreadBitMap {
   return 0;
}

auto MultiThreadScheduler::startThread(TransactionContext& context, ThreadId tid) -> Status {
   BUG_CHECK(tid < limits);
   BUG_CHECK(threads[tid] == nullptr);

   threads[tid] = createThread(tid);
   if(threads[tid] == nullptr) {
      return Result::FATAL_BUG;
   }

   alive++;

   auto status = threads[tid]->exec(context);
   if(status != Result::CONTINUE) {
      THREAD_DONE(tid);
      return status;
   }

   return Result::SUCCESS;
}

TSL_NS_END