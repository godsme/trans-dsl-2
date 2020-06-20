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

TSL_NS_BEGIN

namespace details {

#if __CONCEPT_ENABLED
#define VOID_PLACEHOLDER
#else
#define VOID_PLACEHOLDER void,
#endif

#if __CONCEPT_ENABLED
   template<size_t V_SIZE, size_t V_ALIGN, SeqInt V_SEQ, typename ... T_ACTIONS>
#else
   template<size_t V_SIZE, size_t V_ALIGN, SeqInt V_SEQ, typename = void, typename ... T_ACTIONS>
#endif
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
         V_SEQ + 1,
         VOID_PLACEHOLDER
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
         V_SEQ + 1,
         VOID_PLACEHOLDER
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

#if __CONCEPT_ENABLED
      template<typename T>
      concept NonEmptyLoopPredConcept = IsNonEmptyLoopPred<T>;

      template<typename T>
      concept EmptyLoopPredConcept = IsEmptyLoopPred<T>;
#else
   template<typename T>
   using EnableIfNonEmptyLoopPred = std::enable_if_t<IsNonEmptyLoopPred<T>>;

   template<typename T>
   using EnableIfEmptyLoopPred = std::enable_if_t<IsEmptyLoopPred<T>>;
#endif

   /////////////////////////////////////////////////////////////////////////////////////////
#if __CONCEPT_ENABLED
   template<size_t V_SIZE, size_t V_ALIGN, SeqInt V_SEQ, NonEmptyLoopPredConcept T_HEAD, typename ... T_TAIL>
   struct GenericLoop_<V_SIZE, V_ALIGN, V_SEQ, T_HEAD, T_TAIL...>
#else
   template<size_t V_SIZE, size_t V_ALIGN, SeqInt V_SEQ, typename T_HEAD, typename ... T_TAIL>
   struct GenericLoop_<V_SIZE, V_ALIGN, V_SEQ, EnableIfNonEmptyLoopPred<T_HEAD>, T_HEAD, T_TAIL...>
#endif
      : GenericLoopPred<LoopPredTraits, V_SIZE, V_ALIGN, V_SEQ, T_HEAD, T_TAIL...>
   {};

   ///////////////////////////////////////////////////////////////////////////////////////

#if __CONCEPT_ENABLED
   template<size_t V_SIZE, size_t V_ALIGN, SeqInt V_SEQ, EmptyLoopPredConcept T_HEAD, typename ... T_TAIL>
   struct GenericLoop_<V_SIZE, V_ALIGN, V_SEQ, T_HEAD, T_TAIL...>
#else
   template<size_t V_SIZE, size_t V_ALIGN, SeqInt V_SEQ, typename T_HEAD, typename ... T_TAIL>
   struct GenericLoop_<V_SIZE, V_ALIGN, V_SEQ, EnableIfEmptyLoopPred<T_HEAD>, T_HEAD, T_TAIL...>
#endif
      : GenericLoopEmpty_<LoopPredTraits, V_SIZE, V_ALIGN, V_SEQ, T_HEAD, T_TAIL...>
   {};

   ///////////////////////////////////////////////////////////////////////////////////////
   template<typename T>
   struct ActionTraits {
      using Action = T;
      constexpr static bool isAction = true;
   };

#if __CONCEPT_ENABLED
   template<size_t V_SIZE, size_t V_ALIGN, SeqInt V_SEQ, SchedActionConcept T_HEAD, typename ... T_TAIL>
   struct GenericLoop_<V_SIZE, V_ALIGN, V_SEQ, T_HEAD, T_TAIL...>
#else
   template<size_t V_SIZE, size_t V_ALIGN, SeqInt V_SEQ, typename T_HEAD, typename ... T_TAIL>
   struct GenericLoop_<V_SIZE, V_ALIGN, V_SEQ, IsSchedAction<T_HEAD>, T_HEAD, T_TAIL...>
#endif
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

   template<uint32_t V_MAX_TIMES, typename ... T_ACTIONS>
   struct LOOP__ {
      static_assert(sizeof...(T_ACTIONS) > 0, "loop cannot be empty");
      using Actions = typename GenericLoop_<0, 0, 0, void, T_ACTIONS...>::Inner;
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

#define __loop(...) TSL_NS::details::LOOP__<1, __VA_ARGS__>::Inner
#define __loop_max(times, ...) TSL_NS::details::LOOP__<times, __VA_ARGS__>::Inner
#define __forever(...) TSL_NS::details::LOOP__<std::numeric_limits<uint32>::max(), __VA_ARGS__>::Inner

#define __break_if(pred, ...) decltype(TSL_NS::details::DeduceLoopPredType<pred, ##__VA_ARGS__>())
#define __until(...) __break_if(__VA_ARGS__)
#define __redo_if(pred) decltype(TSL_NS::details::DeduceLoopPredType<pred, TSL_NS::Result::RESTART_REQUIRED>())
#define __while(pred, ...) __break_if(__not(pred), ##__VA_ARGS__)

TSL_NS_END

#endif //TRANS_DSL_2_LOOPHELPER_H
