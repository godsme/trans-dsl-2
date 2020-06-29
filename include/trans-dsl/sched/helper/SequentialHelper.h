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
   struct Sequential final : SchedSequential {
   private:
      enum {
         Num_Of_Actions = sizeof...(T_ACTIONS)
      };
      static_assert(Num_Of_Actions >= 2, "__sequential must contain at least 2 actions");
      static_assert(Num_Of_Actions <= 20, "too many actions in a __sequential");

   private:
      enum {
         Size  = ( MaxSizeCalc{} << ... << sizeof(T_ACTIONS) ),
         Align = ( MaxSizeCalc{} << ... << alignof(T_ACTIONS) )
      };
      alignas(Align) char cache[Size];

      template <SeqInt N>
      auto get() -> SchedAction* {
         if constexpr(N < sizeof...(T_ACTIONS)) {
            using Action = TypeExtractor_t<N, Head, T_ACTIONS...>;
            return new (cache) Action;
         } else {
            return nullptr;
         }
      }

#define Seq_GeT_AcTiOn__(n) case n: return get<n>()

// This would make code smaller
#define ENABLE_SEQ_CODE_GEN 1
#include <trans-dsl/sched/helper/SeqCodeGen.h>

   private:
      OVERRIDE(getNumOfActions()->SeqInt) {
         return Num_Of_Actions;
      }

      OVERRIDE(getNext(SeqInt index) -> SchedAction*) {
#if ENABLE_SEQ_CODE_GEN
         return get_<Num_Of_Actions>(index);
#else
         // switch-case would generate jump table,
         // performance would be better than recursion.
         switch (index) {
            Seq_GeT_AcTiOn__(0);
            Seq_GeT_AcTiOn__(1);
            Seq_GeT_AcTiOn__(2);
            Seq_GeT_AcTiOn__(3);
            Seq_GeT_AcTiOn__(4);
            Seq_GeT_AcTiOn__(5);
            Seq_GeT_AcTiOn__(6);
            Seq_GeT_AcTiOn__(7);
            Seq_GeT_AcTiOn__(8);
            Seq_GeT_AcTiOn__(9);
            Seq_GeT_AcTiOn__(10);
            Seq_GeT_AcTiOn__(11);
            Seq_GeT_AcTiOn__(12);
            Seq_GeT_AcTiOn__(13);
            Seq_GeT_AcTiOn__(14);
            Seq_GeT_AcTiOn__(15);
            Seq_GeT_AcTiOn__(16);
            Seq_GeT_AcTiOn__(17);
            Seq_GeT_AcTiOn__(18);
            Seq_GeT_AcTiOn__(19);
            default:
               return nullptr;
         }
#endif
      }
   };
}

#define __sequential(...) TSL_NS::details::Sequential<__VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_SEQUENTIALHELPER_H
