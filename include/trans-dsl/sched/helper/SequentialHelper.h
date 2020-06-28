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
            using Action = typename TypeExtractor<N, Head, T_ACTIONS...>::type;
            return new (cache) Action;
         } else {
            return nullptr;
         }
      }

   private:
      OVERRIDE(getNumOfActions()->SeqInt) {
         return NUM_OF_ACTIONS;
      }

#define GeT_AcTiOn__(n) case n: return get<n>()

      OVERRIDE(getNext(SeqInt index) -> SchedAction*) {
         // switch-case would generate jump table,
         // performance would be better than recursion.
         switch (index) {
            GeT_AcTiOn__(0);
            GeT_AcTiOn__(1);
            GeT_AcTiOn__(2);
            GeT_AcTiOn__(3);
            GeT_AcTiOn__(4);
            GeT_AcTiOn__(5);
            GeT_AcTiOn__(6);
            GeT_AcTiOn__(7);
            GeT_AcTiOn__(8);
            GeT_AcTiOn__(9);
            GeT_AcTiOn__(10);
            GeT_AcTiOn__(11);
            GeT_AcTiOn__(12);
            GeT_AcTiOn__(13);
            GeT_AcTiOn__(14);
            GeT_AcTiOn__(15);
            GeT_AcTiOn__(16);
            GeT_AcTiOn__(17);
            GeT_AcTiOn__(18);
            GeT_AcTiOn__(19);
            default:
               return nullptr;
         }
      }
   };
}

#define __sequential(...) TSL_NS::details::Sequential<__VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_SEQUENTIALHELPER_H
