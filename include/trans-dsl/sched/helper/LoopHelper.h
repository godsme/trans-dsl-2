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

TSL_NS_BEGIN

namespace details {

   template<size_t V_SIZE, size_t V_ALIGN, SeqInt V_SEQ  VOID_CONCEPT, typename ... T_ACTIONS>
   struct GenericLoop_;

   ///////////////////////////////////////////////////////////////
   template<
      template<typename T> typename T_TRAITS,
      size_t V_SIZE,
      size_t V_ALIGN,
      SeqInt V_SEQ,
      typename T_HEAD,
      typename ... T_TAIL>
   struct GenericLoopPred {
      using Pred = typename T_TRAITS<T_HEAD>::Action;
      using Next =
      typename GenericLoop_<
         V_SIZE,
         V_ALIGN,
         V_SEQ + 1
         VOID_PLACEHOLDER,
         T_TAIL...>::Inner;

      struct Inner : Next {
         using Next::cache;

         auto get(SeqInt seq, bool& isAction) -> SchedAction* {
            if(seq == V_SEQ) {
               isAction = false;
               return &pred;
            } else {
               return Next::get(seq, isAction);
            }
         }

      private:
         Pred pred;
      };
   };

   ///////////////////////////////////////////////////////////////////////////////////////
   template<
      template<typename T> typename T_TRAITS,
      size_t V_SIZE,
      size_t V_ALIGN,
      SeqInt V_SEQ,
      typename T_HEAD,
      typename ... T_TAIL>
   struct GenericLoopEmpty_ {
      using Action = typename T_TRAITS<T_HEAD>::Action;
      using Next =
      typename GenericLoop_<
         std::max(V_SIZE, sizeof(Action)),
         std::max(V_ALIGN, alignof(Action)),
         V_SEQ + 1
         VOID_PLACEHOLDER,
         T_TAIL...>::Inner;

      struct Inner : Next {
         using Next::cache;

         auto get(SeqInt seq, bool& isAction) -> SchedAction* {
            if(seq == V_SEQ) {
               isAction = T_TRAITS<T_HEAD>::isAction;
               return new (cache) Action;
            } else {
               return Next::get(seq, isAction);
            }
         }
      };
   };

   /////////////////////////////////////////////////////////////////////////////////////////
   template<typename T_PRED>
   struct LoopPredTraits {
      using Action = GenericLoopAction<T_PRED>;
      constexpr static bool isAction = false;
   };

   template<typename T>
   constexpr bool IsLoopPred = std::is_base_of_v<LoopPredSignature, T>;

   template<typename T>
   constexpr bool IsNonEmptyLoopPred = IsLoopPred<T> && (sizeof(T) > 1);

   template<typename T>
   constexpr bool IsEmptyLoopPred = IsLoopPred<T> && (sizeof(T) == 1);

   template<typename T>
   DEF_CONCEPT(NonEmptyLoopPredConcept, IsNonEmptyLoopPred<T>);

   template<typename T>
   DEF_CONCEPT(EmptyLoopPredConcept,IsEmptyLoopPred<T>);

   /////////////////////////////////////////////////////////////////////////////////////////
   template<
      size_t V_SIZE,
      size_t V_ALIGN,
      SeqInt V_SEQ,
      CONCEPT_C(NonEmptyLoopPredConcept, T_HEAD),
      typename ... T_TAIL>
   struct GenericLoop_<
      V_SIZE,
      V_ALIGN,
      V_SEQ
      ENABLE_C(NonEmptyLoopPredConcept, T_HEAD),
      T_HEAD,
      T_TAIL...>
      : GenericLoopPred<LoopPredTraits, V_SIZE, V_ALIGN, V_SEQ, T_HEAD, T_TAIL...>
   {};

   ///////////////////////////////////////////////////////////////////////////////////////

   template<
      size_t V_SIZE,
      size_t V_ALIGN,
      SeqInt V_SEQ,
      CONCEPT_C(EmptyLoopPredConcept, T_HEAD),
      typename ... T_TAIL>
   struct GenericLoop_<
      V_SIZE,
      V_ALIGN,
      V_SEQ
      ENABLE_C(EmptyLoopPredConcept, T_HEAD),
      T_HEAD,
      T_TAIL...>
      : GenericLoopEmpty_<LoopPredTraits, V_SIZE, V_ALIGN, V_SEQ, T_HEAD, T_TAIL...>
   {};

   ///////////////////////////////////////////////////////////////////////////////////////
   template<typename T>
   struct ActionTraits {
      using Action = T;
      constexpr static bool isAction = true;
   };

   template<
      size_t V_SIZE,
      size_t V_ALIGN,
      SeqInt V_SEQ,
      CONCEPT_C(SchedActionConcept, T_HEAD),
      typename ... T_TAIL>
   struct GenericLoop_<
      V_SIZE,
      V_ALIGN,
      V_SEQ
      ENABLE_C(SchedActionConcept, T_HEAD),
      T_HEAD,
      T_TAIL...>
      : GenericLoopEmpty_<ActionTraits, V_SIZE, V_ALIGN, V_SEQ, T_HEAD, T_TAIL...>{};

   /////////////////////////////////////////////////////////////////////////////////////////////
   template<size_t T_SIZE, size_t T_ALIGN, SeqInt T_SEQ>
   struct GenericLoop_<T_SIZE, T_ALIGN, T_SEQ> {
      struct Inner  {
         auto get(SeqInt seq, bool& isAction) -> SchedAction* {
            return nullptr;
         }
      protected:
         alignas(T_ALIGN) char cache[T_SIZE];
      };
   };

   template<typename T>
   DEF_CONCEPT(LoopElemConcept, IsLoopPred<T> || IsSchedAction<T>);

   template<uint32_t V_MAX_TIMES, CONCEPT(LoopElemConcept) ... T_ACTIONS>
   struct Loop {
      static_assert(sizeof...(T_ACTIONS) > 0, "loop cannot be empty");
      using Actions = typename GenericLoop_<0, 0, 0 VOID_PLACEHOLDER, T_ACTIONS...>::Inner;
      struct Inner : private Actions, SchedLoop {
      private:
         OVERRIDE(getMaxTime() const -> uint32_t) {
            return V_MAX_TIMES;
         }

         OVERRIDE(getAction(SeqInt seq, bool& isAction) -> SchedAction*) {
            return Actions::get(seq, isAction);
         }
      };
   };
}

#define __loop(...) TSL_NS::details::Loop<1, __VA_ARGS__>::Inner
#define __loop_max(times, ...) TSL_NS::details::LOOP__<times, __VA_ARGS__>::Inner
#define __forever(...) TSL_NS::details::LOOP__<std::numeric_limits<uint32>::max(), __VA_ARGS__>::Inner

#define __break_if(pred, ...) decltype(TSL_NS::details::DeduceLoopPredType<pred, ##__VA_ARGS__>())
#define __until(...) __break_if(__VA_ARGS__)
#define __redo_if(pred) decltype(TSL_NS::details::DeduceLoopPredType<pred, TSL_NS::Result::RESTART_REQUIRED>())
#define __while(pred, ...) __break_if(__not(pred), ##__VA_ARGS__)

TSL_NS_END

#endif //TRANS_DSL_2_LOOPHELPER_H
