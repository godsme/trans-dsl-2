//
// Created by Darwin Yuan on 2020/7/2.
//

#ifndef TRANS_DSL_2_THREADACTIONTRAIT_H
#define TRANS_DSL_2_THREADACTIONTRAIT_H

#include <trans-dsl/tsl_ns.h>
#include <type_traits>
#include <trans-dsl/sched/concepts/ConceptHelper.h>
#include <trans-dsl/sched/domain/ThreadId.h>
#include <cub/type-list/TypeListFold.h>
#include <cub/type-list/TypeListTransform.h>
#include <algorithm>

TSL_NS_BEGIN

struct SchedAction;

namespace details {
   template<typename T, typename = void>
   struct ThreadCreatorTrait_ {
      using type = void;
   };

   template<typename T>
   struct ThreadCreatorTrait_<T, std::void_t<typename T::ThreadActionCreator>> {
      using type = typename T::ThreadActionCreator;
   };

   template<typename T>
   using ThreadCreatorTrait = ThreadCreatorTrait_<T, void>;
}

namespace details {
   template<typename ... Ts>
   class ThreadCreator {
      template<typename T1, typename T2>
      struct Combine {
         struct type : private T1, private T2 {
            static constexpr uint8_t numOfThreads = T1::numOfThreads + T2::numOfThreads;
            static constexpr ThreadId threadId = std::max(T1::threadId, T2::threadId);
            auto createThreadAction(ThreadId tid) -> SchedAction* {
               auto action = T1::createThreadAction(tid);
               return action == nullptr ? T2::createThreadAction(tid) : action;
            }
         };
      };

      template<typename ... Tss>
      using CombineAll = CUB_NS::FoldROpt_t<Combine, Tss...>;

   public:
      using type = CUB_NS::Transform_t<ThreadCreatorTrait, CombineAll, Ts...>;
   };

   template<typename ... Ts>
   using ThreadCreator_t = typename ThreadCreator<Ts...>::type;
}

namespace details {
   template<typename T, typename = void>
   struct FinalThreadCreator {
      static constexpr uint8_t numOfThreads = ThreadCreator_t<T>::numOfThreads;
      static constexpr ThreadId threadId = ThreadCreator_t<T>::threadId;
      auto createThreadAction(ThreadId tid) -> SchedAction* {
         return creator.createThreadAction(tid);
      }
   private:
      ThreadCreator_t<T> creator;
   };

   template<typename T>
   struct FinalThreadCreator<T, std::enable_if_t<std::is_void_v<ThreadCreator_t<T>>>> {
      static constexpr uint8_t numOfThreads = 0;
      static constexpr ThreadId threadId = 0; // main thread id
      auto createThreadAction(ThreadId) -> SchedAction* {
         return nullptr;
      }
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_THREADACTIONTRAIT_H
