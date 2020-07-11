//
// Created by Darwin Yuan on 2020/6/29.
//

#ifndef TRANS_DSL_2_VOLATILESEQ_H
#define TRANS_DSL_2_VOLATILESEQ_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/sched/helper/MaxSize.h>
#include <cub/utils/RepeatMacros.h>
#include <cub/type-list/TypeListTakeRight.h>

TSL_NS_BEGIN

namespace details {
   template<typename T_ELEM, typename ... Ts >
   class VolatileSeq  {
      static constexpr size_t Num_Of_Elements = sizeof...(Ts);
      enum {
         Size  = ( MaxSize{} << ... << sizeof(Ts) ),
         Align = ( MaxSize{} << ... << alignof(Ts) )
      };
      alignas(Align) char cache[Size];

      template <SeqInt N>
      auto get() -> T_ELEM* {
         if constexpr(N < sizeof...(Ts)) {
            using Elem = CUB_NS::Elem_t<N, Ts...>;
            CONCEPT_ASSERT(std::is_base_of_v<T_ELEM, Elem>);
            return new (cache) Elem;
         } else {
            return nullptr;
         }
      }

      ///////////////////////////////////////////////////////////////////
#define Seq_GeT_AcTiOn__(n) case n: return get<n>();
#define Seq_AcTiOn(n) { switch (seq) { SIMPLE_REPEAT(n, Seq_GeT_AcTiOn__) }}
#define Seq_AcTiOn_DeCl(n) if constexpr(Num_Of_Elements <= n) Seq_AcTiOn(n)
#define And_Seq_AcTiOn_DeCl(n) else if constexpr(Num_Of_Elements == n) Seq_AcTiOn(n)
      ///////////////////////////////////////////////////////////////////

   public:
      // Use if-constexpr to avoid unnecessary function template instantiation.
      // Use switch-case to avoid recursion, and the generated jump-table by
      // switch-case is fast.
      auto get(SeqInt seq) -> T_ELEM* {
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
         And_Seq_AcTiOn_DeCl(20)
         And_Seq_AcTiOn_DeCl(21)
         And_Seq_AcTiOn_DeCl(22)
         And_Seq_AcTiOn_DeCl(23)
         And_Seq_AcTiOn_DeCl(24)
         And_Seq_AcTiOn_DeCl(25)
         And_Seq_AcTiOn_DeCl(26)
         And_Seq_AcTiOn_DeCl(27)
         And_Seq_AcTiOn_DeCl(28)
         And_Seq_AcTiOn_DeCl(29)
         And_Seq_AcTiOn_DeCl(30)
         And_Seq_AcTiOn_DeCl(31)
         And_Seq_AcTiOn_DeCl(32)
         And_Seq_AcTiOn_DeCl(33)
         And_Seq_AcTiOn_DeCl(34)
         And_Seq_AcTiOn_DeCl(35)
         And_Seq_AcTiOn_DeCl(36)
         And_Seq_AcTiOn_DeCl(37)
         And_Seq_AcTiOn_DeCl(38)
         And_Seq_AcTiOn_DeCl(39)
         And_Seq_AcTiOn_DeCl(40)
         And_Seq_AcTiOn_DeCl(41)
         And_Seq_AcTiOn_DeCl(42)
         And_Seq_AcTiOn_DeCl(43)
         And_Seq_AcTiOn_DeCl(44)
         And_Seq_AcTiOn_DeCl(45)
         And_Seq_AcTiOn_DeCl(46)
         And_Seq_AcTiOn_DeCl(47)
         And_Seq_AcTiOn_DeCl(48)
         And_Seq_AcTiOn_DeCl(49)
         And_Seq_AcTiOn_DeCl(50)
         return nullptr;
      }
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_VOLATILESEQ_H
