//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_SWITCHCASEHELPER_H
#define TRANS_DSL_2_SWITCHCASEHELPER_H

#include <trans-dsl/sched/action/SchedSwitchCase.h>
#include <trans-dsl/sched/action/ActionPath.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <cub/base/IsClass.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/sched/helper/Pred.h>
#include <trans-dsl/sched/helper/MaxSizeCalc.h>
#include <trans-dsl/sched/helper/TypeExtractor.h>
#include <cub/utils/RepeatMacros.h>

TSL_NS_BEGIN

namespace details {

   template<typename T_PRED, typename T_ACTION VOID_CONCEPT>
   struct GenericActionPathClass;

   template<
      typename T_PRED,
      CONCEPT_C(SchedActionConcept, T_ACTION)>
   struct GenericActionPathClass<
      T_PRED,
      T_ACTION
      ENABLE_C(SchedActionConcept, T_ACTION)> : ActionPath {

      OVERRIDE(shouldExecute(TransactionInfo const& trans) -> bool) {
         auto pred = new (cache) T_PRED;
         return (*pred)(trans);
      }

      OVERRIDE(getAction() -> SchedAction&) {
         auto action = new (cache) T_ACTION;
         return *action;
      }

   private:
      alignas(std::max(alignof(T_PRED), alignof(T_ACTION)))
      char cache[std::max(sizeof(T_PRED), sizeof(T_ACTION))];
   };

   template<PredFunction V_PRED, typename T_ACTION>
   struct GenericActionPathFunc : ActionPath {
      OVERRIDE(shouldExecute(TransactionInfo const& trans) -> bool) {
         return V_PRED(trans);
      }

      OVERRIDE(getAction() -> SchedAction&) {
         auto action = new (cache) T_ACTION;
         return *action;
      }

   private:
      alignas(T_ACTION) char cache[sizeof(T_ACTION)];
   };

   template<typename T_PRED, typename T_ACTION>
   auto DeduceActionPath() -> GenericActionPathClass<T_PRED, T_ACTION>;

   template<PredFunction V_PRED, typename T_ACTION>
   auto DeduceActionPath() -> GenericActionPathFunc<V_PRED, T_ACTION>;

   //////////////////////////////////////////////////////////////////////////////////////////

   template<typename T>
   DEF_CONCEPT(ActionPathConcept, std::is_base_of_v<ActionPath, T>);


   // 24 bytes
   template<CONCEPT(ActionPathConcept) ... T_PATHS>
   struct Switch final : SchedSwitchCase {
      static constexpr size_t Num_Of_Paths = sizeof...(T_PATHS);
      static_assert(Num_Of_Paths >= 2, "should have at least 2 paths, or use __optional instead");
      static_assert(Num_Of_Paths <= 20, "too much paths in one ___switch");

      enum {
         Size  = ( MaxSizeCalc{} << ... << sizeof(T_PATHS) ),
         Align = ( MaxSizeCalc{} << ... << alignof(T_PATHS) )
      };
      alignas(Align) char cache[Size];

      template <SeqInt N>
      auto get() -> ActionPath* {
         if constexpr(N < Num_Of_Paths) {
            using Path = TypeExtractor_t<N, Head, T_PATHS...>;
#if !__CONCEPT_ENABLED
            static_assert(ActionPathConcept<Path>);
#endif
            return new (cache) Path;
         } else {
            return nullptr;
         }
      }

   private:

      ///////////////////////////////////////////////////////////////
      #define AcTiOn_PaTh_GeT_PaTh__(n) case n: return get<n>();
      #define AcTiOn_PaThs(n) switch (seq) { SIMPLE_REPEAT(n, AcTiOn_PaTh_GeT_PaTh__) }
      #define AcTiOn_PaThs_DeCl(n) if constexpr(Num_Of_Paths <= n) { AcTiOn_PaThs(n) }
      #define And_AcTiOn_PaThs_DeCl(n) else AcTiOn_PaThs_DeCl(n)
     ///////////////////////////////////////////////////////////////
     SeqInt i = 0;

      // Use switch-case to avoid recursion
      // Use if-constexpr to avoid unnecessary function template instantiation.
      OVERRIDE(getNext() -> ActionPath *) {
         SeqInt seq = i++;

         AcTiOn_PaThs_DeCl(2)
         And_AcTiOn_PaThs_DeCl(3)
         And_AcTiOn_PaThs_DeCl(4)
         And_AcTiOn_PaThs_DeCl(5)
         And_AcTiOn_PaThs_DeCl(6)
         And_AcTiOn_PaThs_DeCl(7)
         And_AcTiOn_PaThs_DeCl(8)
         And_AcTiOn_PaThs_DeCl(9)
         And_AcTiOn_PaThs_DeCl(10)
         And_AcTiOn_PaThs_DeCl(11)
         And_AcTiOn_PaThs_DeCl(12)
         And_AcTiOn_PaThs_DeCl(13)
         And_AcTiOn_PaThs_DeCl(14)
         And_AcTiOn_PaThs_DeCl(15)
         And_AcTiOn_PaThs_DeCl(16)
         And_AcTiOn_PaThs_DeCl(17)
         And_AcTiOn_PaThs_DeCl(18)
         And_AcTiOn_PaThs_DeCl(19)

         return nullptr;
      }
   };

   inline auto IsTrue__(const TransactionInfo&) -> bool { return true; }
}

TSL_NS_END

#define __case(pred, action) decltype(TSL_NS::details::DeduceActionPath<pred, action>())
#define __otherwise(action) __case(TSL_NS::details::IsTrue__, action)
#define __switch(...) TSL_NS::details::Switch<__VA_ARGS__>

#endif //TRANS_DSL_2_SWITCHCASEHELPER_H
