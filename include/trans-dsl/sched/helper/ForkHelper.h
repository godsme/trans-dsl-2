//
// Created by Darwin Yuan on 2020/7/1.
//

#ifndef TRANS_DSL_2_FORKHELPER_H
#define TRANS_DSL_2_FORKHELPER_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/sched/domain/ThreadId.h>
#include <trans-dsl/sched/action/SchedFork.h>
#include <trans-dsl/sched/concepts/ConceptHelper.h>
#include <trans-dsl/sched/domain/ThreadBitMap.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>

TSL_NS_BEGIN

namespace details {
   template <ThreadId TID, CONCEPT(SchedActionConcept) T_ACTION>
   class Fork final : public SchedFork {
      static_assert(TID < ThreadBitMap::max, "specified Thread ID is out of scope");
      static_assert(TID != 0, "0 is main thread, which has already been created");
      CONCEPT_ASSERT(SchedActionConcept<T_ACTION>);

   private:
      struct Inner {
         struct ThreadActionCreator {
            static constexpr ThreadId threadId = TID;

            auto createThreadAction(ThreadId tid) -> SchedAction * {
               return (tid == TID) ? new(cache) T_ACTION : nullptr;
            }

         private:
            alignas(alignof(T_ACTION)) char cache[sizeof(T_ACTION)];
         };
      };

   public:
      using ThreadActionCreator = ThreadCreator_t<Inner, T_ACTION>;

   private:
      OVERRIDE(getThreadId() const -> ThreadId) { return TID; }

   };
}

#define __fork(tid, ...) TSL_NS::details::Fork<tid, __VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_FORKHELPER_H
