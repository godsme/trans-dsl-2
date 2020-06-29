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
#include <cub/utils/RepeatMacros.h>

TSL_NS_BEGIN

struct SchedAction;

namespace details {
   template<CONCEPT(SchedActionConcept) ... T_ACTIONS>
   class Sequential final : public SchedSequential {
      enum { Num_Of_Actions = sizeof...(T_ACTIONS) };
      static_assert(Num_Of_Actions >= 2, "__sequential must contain at least 2 actions");
      static_assert(Num_Of_Actions <= 20, "too many actions in a __sequential");

      enum {
         Size  = ( MaxSizeCalc{} << ... << sizeof(T_ACTIONS) ),
         Align = ( MaxSizeCalc{} << ... << alignof(T_ACTIONS) )
      };
      alignas(Align) char cache[Size];

      template <SeqInt N>
      auto get() -> SchedAction* {
         if constexpr(N < sizeof...(T_ACTIONS)) {
            using Action = TypeExtractor_t<N, Head, T_ACTIONS...>;
            #if !__CONCEPT_ENABLED
            static_assert(SchedActionConcept<Action>);
            #endif
            return new (cache) Action;
         } else {
            return nullptr;
         }
      }

   private:
      OVERRIDE(getNumOfActions()->SeqInt) { return Num_Of_Actions; }

      ///////////////////////////////////////////////////////////////////
      #define Seq_GeT_AcTiOn__(n) case n: return get<n>();
      #define Seq_AcTiOn(n) { switch (seq) { SIMPLE_REPEAT(n, Seq_GeT_AcTiOn__) }}
      #define Seq_AcTiOn_DeCl(n) if constexpr(Num_Of_Actions <= n) Seq_AcTiOn(n)
      #define And_Seq_AcTiOn_DeCl(n) else if constexpr(Num_Of_Actions == n) Seq_AcTiOn(n)
      ///////////////////////////////////////////////////////////////////

      // Use if-constexpr to avoid unnecessary function template instantiation.
      // Use switch-case to avoid recursion, and the generated jump-table by
      // switch-case is fast.
      OVERRIDE(getNext(SeqInt seq) -> SchedAction*) {
         Seq_AcTiOn_DeCl(2)
         And_Seq_AcTiOn_DeCl(3)
         And_Seq_AcTiOn_DeCl(4)
         And_Seq_AcTiOn_DeCl(5)
         And_Seq_AcTiOn_DeCl(6)
         And_Seq_AcTiOn_DeCl(7)
         And_Seq_AcTiOn_DeCl(8)
         And_Seq_AcTiOn_DeCl(9)
         And_Seq_AcTiOn_DeCl(10)
         And_Seq_AcTiOn_DeCl(11)
         And_Seq_AcTiOn_DeCl(12)
         And_Seq_AcTiOn_DeCl(13)
         And_Seq_AcTiOn_DeCl(14)
         And_Seq_AcTiOn_DeCl(15)
         And_Seq_AcTiOn_DeCl(16)
         And_Seq_AcTiOn_DeCl(17)
         And_Seq_AcTiOn_DeCl(18)
         And_Seq_AcTiOn_DeCl(19)

         return nullptr;
      }
   };
}

#define __sequential(...) TSL_NS::details::Sequential<__VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_SEQUENTIALHELPER_H
