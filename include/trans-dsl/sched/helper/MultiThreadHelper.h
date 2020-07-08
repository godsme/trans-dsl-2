//
// Created by Darwin Yuan on 2020/7/4.
//

#ifndef TRANS_DSL_2_MULTITHREADHELPER_H
#define TRANS_DSL_2_MULTITHREADHELPER_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/action/SchedMultiThread.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/domain/TransListenerObservedAids.h>

TSL_NS_BEGIN

namespace details {

   template<typename MAIN_ACTION>
   struct MultiThread {
      template<TransListenerObservedAids const& AIDs>
      struct ActionRealType : SchedMultiThread {
         auto exec(TransactionContext &context) -> Status {
            return SchedMultiThread::start(context, mainThreadAction);
         }

      private:
         OVERRIDE(createThread(ThreadId tid) -> SchedAction*) {
            return threadCreator.createThreadAction(tid);
         }

         OVERRIDE(getMaxThreads() const -> uint8_t) {
            return MAX_NUM_OF_THREADS;
         }

         OVERRIDE(getThreads()->Threads) {
            return threads;
         }

         using MainAction = ActionRealTypeTraits_t<AIDs, MAIN_ACTION>;

         enum : uint8_t { MAX_NUM_OF_THREADS = details::FinalThreadCreator<MainAction>::numOfThreads + 1 };
         static_assert(MAX_NUM_OF_THREADS <= ThreadBitMap::max, "the specified tid is out of scope");
         static_assert(MAX_NUM_OF_THREADS == details::FinalThreadCreator<MainAction>::threadId + 1,
                       "you should use consecutive thread IDs to save memory & speed up this transaction");

         SchedAction *threads[MAX_NUM_OF_THREADS]{};

         MainAction mainThreadAction;
         details::FinalThreadCreator<MainAction> threadCreator;

         static_assert(MAX_NUM_OF_THREADS > 1, "the specified tid is out of scope");
      };
   };

   template<typename T>
   using MultiThread_t = typename MultiThread<T>::template ActionRealType<EmptyAids>;
}

TSL_NS_END

#define __multi_thread(...) TSL_NS::details::MultiThread<TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>>
#define __def_multi_thread(...) TSL_NS::details::MultiThread_t<TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>>

#endif //TRANS_DSL_2_MULTITHREADHELPER_H
