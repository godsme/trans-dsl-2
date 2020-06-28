//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_LOOPHELPER_H
#define TRANS_DSL_2_LOOPHELPER_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/sched/helper/LoopPred.h>
#include <trans-dsl/sched/helper/IsSchedAction.h>
#include <trans-dsl/sched/helper/LoopPredAction.h>
#include <trans-dsl/sched/action/SchedLoop.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/sched/concepts/ConceptHelper.h>
#include <trans-dsl/sched/helper/MaxSizeCalc.h>
#include <trans-dsl/sched/helper/TypeExtractor.h>

TSL_NS_BEGIN

namespace details {

   template<VOID_CONCEPT_2 typename ... T_ACTIONS>
   struct GenericLoop_;

   ///////////////////////////////////////////////////////////////
   template<
      template<typename T> typename T_TRAITS,
      typename T_HEAD,
      typename ... T_TAIL>
   struct GenericLoopPred : GenericLoop_< VOID_PLACEHOLDER_2 T_TAIL...> {
      auto get(char*, bool& isAction) -> SchedAction* {
            isAction = false;
            return &pred;
      }

   private:
      using Pred = typename T_TRAITS<T_HEAD>::Action;
      Pred pred;
   };

   ///////////////////////////////////////////////////////////////////////////////////////
   template<
      template<typename T> typename T_TRAITS,
      typename T_HEAD,
      typename ... T_TAIL>
   struct GenericLoopEmpty_ : GenericLoop_<VOID_PLACEHOLDER_2 T_TAIL...> {
      auto get(char* cache, bool& isAction) -> SchedAction* {
         isAction = T_TRAITS<T_HEAD>::isAction;
         using Action = typename T_TRAITS<T_HEAD>::Action;
         return new (cache) Action;
      }
   };

   /////////////////////////////////////////////////////////////////////////////////////////
   template<typename T_PRED>
   struct LoopPredTraits {
      using Action = GenericLoopAction<T_PRED>;
      constexpr static bool isAction = false;
   };

   template<typename T>
   DEF_CONCEPT(LoopPredConcept, std::is_base_of_v<LoopPredSignature, T>);

   template<typename T>
   DEF_CONCEPT(NonEmptyLoopPredConcept, LoopPredConcept<T> && (sizeof(T) > 1));

   template<typename T>
   DEF_CONCEPT(EmptyLoopPredConcept, LoopPredConcept<T> && (sizeof(T) == 1));

   /////////////////////////////////////////////////////////////////////////////////////////
   template<
      CONCEPT_C(NonEmptyLoopPredConcept, T_HEAD),
      typename ... T_TAIL>
   struct GenericLoop_<
      ENABLE_C_2(NonEmptyLoopPredConcept, T_HEAD)
      T_HEAD,
      T_TAIL...>
      : GenericLoopPred<LoopPredTraits, T_HEAD, T_TAIL...>
   {};

   ///////////////////////////////////////////////////////////////////////////////////////

   template<
      CONCEPT_C(EmptyLoopPredConcept, T_HEAD),
      typename ... T_TAIL>
   struct GenericLoop_<
      ENABLE_C_2(EmptyLoopPredConcept, T_HEAD)
      T_HEAD,
      T_TAIL...>
      : GenericLoopEmpty_<LoopPredTraits, T_HEAD, T_TAIL...>
   {};

   ///////////////////////////////////////////////////////////////////////////////////////
   template<typename T>
   struct ActionTraits {
      using Action = T;
      constexpr static bool isAction = true;
   };

   template<
      CONCEPT_C(SchedActionConcept, T_HEAD),
      typename ... T_TAIL>
   struct GenericLoop_<
      ENABLE_C_2(SchedActionConcept, T_HEAD)
      T_HEAD,
      T_TAIL...>
      : GenericLoopEmpty_<ActionTraits, T_HEAD, T_TAIL...>{};

   /////////////////////////////////////////////////////////////////////////////////////////////
   template<>
   struct GenericLoop_<> {
      struct Inner  {
         auto get(char*, bool&) -> SchedAction* {
            return nullptr;
         }
      };
   };

   template<typename T>
   DEF_CONCEPT(LoopElemConcept, LoopPredConcept<T> || SchedActionConcept<T>);

   template<uint32_t V_MAX_TIMES, CONCEPT(LoopElemConcept) ... T_ACTIONS>
   struct Loop : private GenericLoop_<VOID_PLACEHOLDER_2 T_ACTIONS...>, SchedLoop {
   private:
      static_assert(sizeof...(T_ACTIONS) > 0, "loop cannot be empty");
      static_assert(sizeof...(T_ACTIONS) <= 20, "too many actions in a loop");

      template <typename T>
      constexpr static size_t SizeOf  = SchedActionConcept<T> ? sizeof(T) : 0 ;

      template <typename T>
      constexpr static size_t AlignOf = SchedActionConcept<T> ? alignof(T) : 0 ;

      enum {
         SIZE  = ( MaxSizeCalc{} << ... << SizeOf<T_ACTIONS> ),
         ALIGN = ( MaxSizeCalc{} << ... << AlignOf<T_ACTIONS> )
      };

      alignas(ALIGN) char cache[SIZE];

      template<typename H, typename ... Tail>
      struct LoopAction {
         using type = GenericLoop_<VOID_PLACEHOLDER_2 H, Tail...>;
      };

      template <size_t N>
      auto get(bool& isAction) -> SchedAction* {
         if constexpr(N < sizeof...(T_ACTIONS)) {
            using Type = typename TypeExtractor<N, LoopAction, T_ACTIONS...>::type;
            return Type::get(cache, isAction);
         } else {
            return nullptr;
         }
      }

   public:
      OVERRIDE(getMaxTime() const -> uint32_t) {
         return V_MAX_TIMES;
      }

#define Seq_GeT_AcTiOn__(n) case n: return get<n>(isAction)

      OVERRIDE(getAction(SeqInt seq, bool& isAction) -> SchedAction*) {
         switch (seq) {
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
      }
   };
}

#define __loop(...) TSL_NS::details::Loop<1, __VA_ARGS__>
#define __loop_max(times, ...) TSL_NS::details::LOOP__<times, __VA_ARGS__>::Inner
#define __forever(...) TSL_NS::details::LOOP__<std::numeric_limits<uint32>::max(), __VA_ARGS__>::Inner

#define __break_if(pred, ...) decltype(TSL_NS::details::DeduceLoopPredType<pred, ##__VA_ARGS__>())
#define __until(...) __break_if(__VA_ARGS__)
#define __redo_if(pred) decltype(TSL_NS::details::DeduceLoopPredType<pred, TSL_NS::Result::RESTART_REQUIRED>())
#define __while(pred, ...) __break_if(__not(pred), ##__VA_ARGS__)

TSL_NS_END

#endif //TRANS_DSL_2_LOOPHELPER_H
