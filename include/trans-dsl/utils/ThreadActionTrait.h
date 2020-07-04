//
// Created by Darwin Yuan on 2020/7/2.
//

#ifndef TRANS_DSL_2_THREADACTIONTRAIT_H
#define TRANS_DSL_2_THREADACTIONTRAIT_H

#include <trans-dsl/tsl_ns.h>
#include <type_traits>
#include <trans-dsl/sched/concepts/ConceptHelper.h>
#include <trans-dsl/sched/domain/ThreadId.h>
#include <algorithm>

TSL_NS_BEGIN

struct SchedAction;

namespace details {
   template<typename T, typename = void>
   struct ThreadCreatorTrait {
      using type = void;
   };

   template<typename T>
   struct ThreadCreatorTrait<T, std::enable_if_t<std::is_class_v<typename T::ThreadActionCreator>>> {
      using type = typename T::ThreadActionCreator;
   };

   template<typename T>
   using ThreadCreatorTrait_t = typename ThreadCreatorTrait<T, void>::type ;
}

namespace details {
   template<typename T1, typename T2, typename = void>
   struct ThreadCreatorCombinator;

   template<typename T1, typename T2>
   struct ThreadCreatorCombinator<T1, T2, std::enable_if_t<std::is_void_v<T2>>> {
      // T1 is not a void for sure, skip T2 if it's void.
      using type = T1;
   };

   template<typename T1, typename T2>
   struct ThreadCreatorCombinator<T1, T2, std::enable_if_t<!std::is_void_v<T2>>> {
      // T1, T2 are both not void, combine them as one type.
      struct type : private T1, private T2 {
         static constexpr uint8_t numOfThreads = T1::numOfThreads + T2::numOfThreads;
         static constexpr ThreadId threadId = std::max(T1::threadId, T2::threadId);
         auto createThreadAction(ThreadId tid) -> SchedAction* {
            auto action = T1::createThreadAction(tid);
            return action == nullptr ? T2::createThreadAction(tid) : action;
         }
      };
   };

   template<typename T1, typename T2>
   using ThreadCreatorCombinator_t = typename ThreadCreatorCombinator<T1, T2, void>::type;
}

namespace details {
   template<typename = void, typename ... Ts>
   struct ThreadCreator;

   template<typename ... Ts>
   using ThreadCreator_t = typename ThreadCreator<void, Ts...>::type;

   template <typename T>
   constexpr bool IsVoidThreadCreator = std::is_void_v<ThreadCreatorTrait_t<T>>;

   template<typename H, typename ... Ts>
   struct ThreadCreator<std::enable_if_t<IsVoidThreadCreator<H>>, H, Ts...> {
      // skip if H is void. But finally we might still get void
      // if all Ts does not contain a fork object, directly or indirectly.
      using type = ThreadCreator_t<Ts...>;
   };

   template<typename H, typename ... Ts>
   struct ThreadCreator<std::enable_if_t<!IsVoidThreadCreator<H>>, H, Ts...> {
      using type = ThreadCreatorCombinator_t<ThreadCreatorTrait_t<H>, ThreadCreator_t<Ts...>>;
   };

   template<>
   struct ThreadCreator<void> {
      using type = void;
   };
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
