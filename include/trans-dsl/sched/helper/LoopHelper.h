//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_LOOPHELPER_H
#define TRANS_DSL_2_LOOPHELPER_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/sched/helper/LoopPred.h>
#include <trans-dsl/sched/helper/LoopPredAction.h>
#include <trans-dsl/sched/action/SchedLoop.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/sched/concepts/ConceptHelper.h>
#include <trans-dsl/sched/helper/MaxSizeCalc.h>
#include <trans-dsl/utils/TypeListExtractor.h>
#include <cub/utils/RepeatMacros.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/helper/InlineSeqHelper.h>

TSL_NS_BEGIN

namespace details {
   template<VOID_CONCEPT_2 typename ... T_ACTIONS>
   struct GenericLoop;

   ///////////////////////////////////////////////////////////////////////////////////////
   template<typename T_HEAD, typename ... T_TAIL>
   struct GenericLoopPredEntry : GenericLoop< VOID_PLACEHOLDER_2 T_TAIL...> {
      auto get(char*, bool& isAction) -> SchedAction* {
         isAction = false;
         return &pred;
      }
   private:
      GenericLoopPredAction<T_HEAD> pred;
   };

   ///////////////////////////////////////////////////////////////////////////////////////
   template<
      template<typename T> typename T_TRAITS,
      typename T_HEAD,
      typename ... T_TAIL>
   struct GenericLoopVolatileEntry : GenericLoop<VOID_PLACEHOLDER_2 T_TAIL...> {
      auto get(char* cache, bool& isAction) -> SchedAction* {
         isAction = T_TRAITS<T_HEAD>::isAction;
         return new (cache) typename T_TRAITS<T_HEAD>::Action;
      }
   };

   /////////////////////////////////////////////////////////////////////////////////////////
   template<typename T_PRED>
   struct LoopPredTraits {
      using Action = GenericLoopPredAction<T_PRED>;
      constexpr static bool isAction = false;
   };

   template<typename T>
   DEF_CONCEPT(LoopPredConcept, std::is_base_of_v<LoopPredSignature, T>);

   template<typename T>
   DEF_CONCEPT(StatefulLoopPredConcept, LoopPredConcept<T> && (sizeof(T) > 1));

   template<typename T>
   DEF_CONCEPT(StatelessLoopPredConcept, LoopPredConcept<T> && (sizeof(T) == 1));

   /////////////////////////////////////////////////////////////////////////////////////////
   template<
      CONCEPT(StatefulLoopPredConcept) T_HEAD,
      typename ... T_TAIL>
   struct GenericLoop<
      ENABLE_C_2(StatefulLoopPredConcept, T_HEAD)
      T_HEAD,
      T_TAIL...>
      : GenericLoopPredEntry<T_HEAD, T_TAIL...> {};

   ///////////////////////////////////////////////////////////////////////////////////////

   template<
      CONCEPT(StatelessLoopPredConcept) T_HEAD,
      typename ... T_TAIL>
   struct GenericLoop<
      ENABLE_C_2(StatelessLoopPredConcept, T_HEAD)
      T_HEAD,
      T_TAIL...>
      : GenericLoopVolatileEntry<LoopPredTraits, T_HEAD, T_TAIL...> {};

   ///////////////////////////////////////////////////////////////////////////////////////
   template<typename T>
   struct ActionTraits {
      using Action = T;
      constexpr static bool isAction = true;
   };

   template<
      CONCEPT(SchedActionConcept) T_HEAD,
      typename ... T_TAIL>
   struct GenericLoop<
      ENABLE_C_2(SchedActionConcept, T_HEAD)
      T_HEAD,
      T_TAIL...>
      : GenericLoopVolatileEntry<ActionTraits, T_HEAD, T_TAIL...>{};

   /////////////////////////////////////////////////////////////////////////////////////////////
   template<>
   struct GenericLoop<> {};

   template<typename T>
   DEF_CONCEPT(LoopEntryConcept, LoopPredConcept<T> || SchedActionConcept<T>);


   template<typename ... T_ENTRIES>
   class LoopBase : public GenericLoop<VOID_PLACEHOLDER_2 T_ENTRIES...> {

      // skip all loop predication.
      template <typename T>
      static constexpr size_t Size_Of  = SchedActionConcept<T> ? sizeof(T) : 0;
      template <typename T>
      static constexpr size_t Align_Of = SchedActionConcept<T> ? alignof(T) : 0;

      static constexpr size_t Align = (MaxSizeCalc{} << ... << Align_Of<T_ENTRIES>);
      static constexpr size_t Size  = (MaxSizeCalc{} << ... << Size_Of<T_ENTRIES>);

      template<typename ... Ts>
      struct LoopEntry {
         using type = GenericLoop<VOID_PLACEHOLDER_2 Ts...>;
      };

   protected:
      enum { Expanded_Num_Of_Entries = sizeof...(T_ENTRIES) };

      alignas(Align) char cache[Size];

      template <SeqInt N>
      auto get(bool& isAction) -> SchedAction* {
         if constexpr(N < Expanded_Num_Of_Entries) {
            return TypeListExtractor_t<N, LoopEntry, T_ENTRIES...>::get(cache, isAction);
         } else {
            return nullptr;
         }
      }
   };

   template<uint32_t V_MAX_TIMES, CONCEPT(LoopEntryConcept) ... T_ENTRIES>
   class Loop final  {
      enum { Num_Of_Entries = sizeof...(T_ENTRIES) };
      static_assert(Num_Of_Entries > 0, "loop cannot be empty");
      static_assert(Num_Of_Entries <= 30, "too many entries in a loop");

      using Base = typename inline_seq::Comb_t<LoopBase, T_ENTRIES...>::type;

   public:

      class Inner : public SchedLoop, Base {
      public:
         using ThreadActionCreator = ThreadCreator_t<T_ENTRIES...>;

      private:
         OVERRIDE(getMaxTime() const -> uint32_t) { return V_MAX_TIMES; }

         ///////////////////////////////////////////////////////////////////////
         #define LoOp_AcTiOn(n) case n: return Base::get<n>(isAction);
         #define LoOp_AcTiOn_BlOcK(n) { switch (seq) { SIMPLE_REPEAT(n, LoOp_AcTiOn) }}
         #define LoOp_AcTiOn_DeCl(n) if constexpr(Base::Expanded_Num_Of_Entries <= n) LoOp_AcTiOn_BlOcK(n)
         #define And_LoOp_AcTiOn_DeCl(n) else if constexpr(Base::Expanded_Num_Of_Entries == n) LoOp_AcTiOn_BlOcK(n)
         ///////////////////////////////////////////////////////////////////////

         // Use if-constexpr to avoid unnecessary function template instantiation.
         // Use switch-case to avoid recursion, and the generated jump-table by
         // switch-case is fast.
         OVERRIDE(getAction(SeqInt seq, bool& isAction) -> SchedAction*) {
            LoOp_AcTiOn_DeCl(2)
            And_LoOp_AcTiOn_DeCl(3)
            And_LoOp_AcTiOn_DeCl(4)
            And_LoOp_AcTiOn_DeCl(5)
            And_LoOp_AcTiOn_DeCl(6)
            And_LoOp_AcTiOn_DeCl(7)
            And_LoOp_AcTiOn_DeCl(8)
            And_LoOp_AcTiOn_DeCl(9)
            And_LoOp_AcTiOn_DeCl(10)
            And_LoOp_AcTiOn_DeCl(11)
            And_LoOp_AcTiOn_DeCl(12)
            And_LoOp_AcTiOn_DeCl(13)
            And_LoOp_AcTiOn_DeCl(14)
            And_LoOp_AcTiOn_DeCl(15)
            And_LoOp_AcTiOn_DeCl(16)
            And_LoOp_AcTiOn_DeCl(17)
            And_LoOp_AcTiOn_DeCl(18)
            And_LoOp_AcTiOn_DeCl(19)
            And_LoOp_AcTiOn_DeCl(20)
            And_LoOp_AcTiOn_DeCl(21)
            And_LoOp_AcTiOn_DeCl(22)
            And_LoOp_AcTiOn_DeCl(23)
            And_LoOp_AcTiOn_DeCl(24)
            And_LoOp_AcTiOn_DeCl(25)
            And_LoOp_AcTiOn_DeCl(26)
            And_LoOp_AcTiOn_DeCl(27)
            And_LoOp_AcTiOn_DeCl(28)
            And_LoOp_AcTiOn_DeCl(29)
            And_LoOp_AcTiOn_DeCl(30)
            return nullptr;
         }


      };
   };

   template<uint32_t V_MAX_TIMES, typename ... T_ENTRIES>
   using Loop_ = typename Loop<V_MAX_TIMES, T_ENTRIES...>::Inner;
}

#define __loop(...) TSL_NS::details::Loop_<1, __VA_ARGS__>
#define __loop_max(times, ...) TSL_NS::details::Loop_<times, __VA_ARGS__>
#define __forever(...) TSL_NS::details::Loop_<std::numeric_limits<uint32>::max(), __VA_ARGS__>

#define __break_if(pred, ...) decltype(TSL_NS::details::DeduceLoopPredType<pred, ##__VA_ARGS__>())
#define __until(...) __break_if(__VA_ARGS__)
#define __redo_if(pred) decltype(TSL_NS::details::DeduceLoopPredType<pred, TSL_NS::Result::RESTART_REQUIRED>())
#define __while(pred, ...) __break_if(__not(pred), ##__VA_ARGS__)

TSL_NS_END

#endif //TRANS_DSL_2_LOOPHELPER_H
