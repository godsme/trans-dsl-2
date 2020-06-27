//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_SEQUENTIALHELPER_H
#define TRANS_DSL_2_SEQUENTIALHELPER_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/action/SchedSequential.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/sched/helper/MaxSizeCalc.h>
#include <trans-dsl/sched/helper/TypeExtractor.h>

TSL_NS_BEGIN

struct SchedAction;

namespace details {
   template<CONCEPT(SchedActionConcept) ... T_ACTIONS>
   struct Sequential : SchedSequential {
   private:
      enum { NUM_OF_ACTIONS = sizeof...(T_ACTIONS) };
      static_assert(NUM_OF_ACTIONS >= 2, "__sequential must contain at least 2 actions");
      static_assert(NUM_OF_ACTIONS <= 20, "too many actions in a __sequential");

   private:
      enum {
         SIZE  = ( MaxSizeCalc{} << ... << sizeof(T_ACTIONS) ),
         ALIGN = ( MaxSizeCalc{} << ... << alignof(T_ACTIONS) )
      };

      alignas(ALIGN) char cache[SIZE];

      template <size_t N>
      auto get() -> SchedAction* {
         if constexpr(N < sizeof...(T_ACTIONS)) {
            using Action = typename TypeExtractor<N, T_ACTIONS...>::type;
            return new (cache) Action;
         } else {
            return nullptr;
         }
      }

   private:
      OVERRIDE(getNumOfActions()->SeqInt) {
         return NUM_OF_ACTIONS;
      }

#define GeT_AcToOn__(n) case n: return get<n>()

      OVERRIDE(getNext(SeqInt index) -> SchedAction*) {
         switch (index) {
            GeT_AcToOn__(0);
            GeT_AcToOn__(1);
            GeT_AcToOn__(2);
            GeT_AcToOn__(3);
            GeT_AcToOn__(4);
            GeT_AcToOn__(5);
            GeT_AcToOn__(6);
            GeT_AcToOn__(7);
            GeT_AcToOn__(8);
            GeT_AcToOn__(9);
            GeT_AcToOn__(10);
            GeT_AcToOn__(11);
            GeT_AcToOn__(12);
            GeT_AcToOn__(13);
            GeT_AcToOn__(14);
            GeT_AcToOn__(15);
            GeT_AcToOn__(16);
            GeT_AcToOn__(17);
            GeT_AcToOn__(18);
            GeT_AcToOn__(19);
            default:
               return nullptr;
         }
      }
   };
}

#define __sequential(...) TSL_NS::details::Sequential<__VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_SEQUENTIALHELPER_H
