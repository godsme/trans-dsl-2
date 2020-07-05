//
// Created by Darwin Yuan on 2020/7/4.
//

#ifndef TRANS_DSL_2_MULTITHREADHELPER_H
#define TRANS_DSL_2_MULTITHREADHELPER_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/action/SchedMultiThread.h>
#include <trans-dsl/utils/ThreadActionTrait.h>

TSL_NS_BEGIN

namespace details {
   template<typename MAIN_ACTION>
   struct MultiThread : SchedMultiThread {
      auto exec(TransactionContext& context) -> Status {
         return SchedMultiThread::start(context, mainThreadAction);
      }

   private:
      OVERRIDE(createThread(ThreadId tid) -> SchedAction*) {
         return threadCreator.createThreadAction(tid);
      }

      OVERRIDE(getMaxThreads() const -> uint8_t) {
         return MAX_NUM_OF_THREADS;
      }

      OVERRIDE(getThreads() -> Threads) {
         return threads;
      }

      enum : uint8_t { MAX_NUM_OF_THREADS = details::FinalThreadCreator<MAIN_ACTION>::numOfThreads + 1 };
      static_assert(MAX_NUM_OF_THREADS <= ThreadBitMap::max, "the specified tid is out of scope");
      static_assert(MAX_NUM_OF_THREADS == details::FinalThreadCreator<MAIN_ACTION>::threadId + 1,
         "you should use consecutive thread IDs to save memory & speed up this transaction");

      static_assert(MAX_NUM_OF_THREADS > 1, "the specified tid is out of scope");

      SchedAction* threads[MAX_NUM_OF_THREADS]{};
      MAIN_ACTION mainThreadAction;
      details::FinalThreadCreator<MAIN_ACTION> threadCreator;
   };
}

TSL_NS_END

#define __multi_thread(...) TSL_NS::details::MultiThread<__VA_ARGS__>

#endif //TRANS_DSL_2_MULTITHREADHELPER_H
