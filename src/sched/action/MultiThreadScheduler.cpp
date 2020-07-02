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

#define FOREACH_THREAD__(i, from) for(ThreadId i = from; i < limits && actions[i] != nullptr; i++)
#define FOREACH_THREAD(i) FOREACH_THREAD__(i, 0)

#define THREAD_DONE(i) actions[i] = nullptr; --alive
#define MAIN_TID 0

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::start(TransactionContext& context, SchedAction& action) -> Status {
   BUG_CHECK(state == State::INIT);

   context.updateMultiThreadContext(*this);
   actions[MAIN_TID] = &action;

   auto status = action.exec(context);
   if(is_working_status(status)) {
      state = State::WORKING;
      ++alive;
   } else {
      state = State::DONE;
   }

   // user config
   limits = MAX_NUM_OF_THREADS;

   return status;
}

///////////////////////////////////////////////////////////////////////////////////////
auto MultiThreadScheduler::handleEvent(ThreadId i, TransactionContext& context, Event const& event) -> Status {
   auto status = actions[i]->handleEvent(context, event);
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
      auto status = actions[i]->stop(context, cause);
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
      actions[i]->kill(context, cause);
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
auto MultiThreadScheduler::slavesHandleEvent(TransactionContext& context, Event const& event) -> Status {
   FOREACH_THREAD__(i, 1) {
      auto status = handleEvent(i, context, event);
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
   auto status = handleEvent(MAIN_TID, context, event);
   if(unlikely(actions[MAIN_TID] == nullptr)) {
      return kill_(context, status);
   }

   if(likely(event.isConsumed() || alive == 1)) {
      return status;
   }

   return slavesHandleEvent(context, event);
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

auto MultiThreadScheduler::startThread(ThreadId, SchedAction&) -> Status {
   return Result::FATAL_BUG;
}

TSL_NS_END