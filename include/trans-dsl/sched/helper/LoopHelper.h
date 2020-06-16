//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_LOOPHELPER_H
#define TRANS_DSL_2_LOOPHELPER_H

#include <trans-dsl/sched/helper/LoopPred.h>
#include <trans-dsl/sched/helper/IsSchedAction.h>
#include <trans-dsl/sched/helper/LoopPredAction.h>
#include <trans-dsl/sched/action/SchedLoop.h>

TSL_NS_BEGIN

namespace details {
   struct BreakSignature {};
   using BreakPred = LoopPred<BreakSignature>;
   using BreakPredAction = LoopPredAction<BreakSignature>;

   struct ContinueSignature {};
   using ContinuePred = LoopPred<ContinueSignature>;
   using ContinuePredAction = LoopPredAction<ContinueSignature>;
}

namespace details {
   using LoopSeq = unsigned short;

   template<size_t V_SIZE, size_t V_ALIGN, LoopSeq V_SEQ, typename = void, typename ... T_ACTIONS>
   struct GenericLoop_;

   template<
      template<typename T> typename T_TRAITS,
      size_t V_SIZE,
      size_t V_ALIGN,
      LoopSeq V_SEQ,
      typename T_HEAD,
      typename ... T_TAIL>
   struct GenericLoopNonEmpty_ {
      using Action = typename T_TRAITS<T_HEAD>::Action;
      using Next =
      typename GenericLoop_<
         V_SIZE,
         V_ALIGN,
         V_SEQ + 1,
         void,
         T_TAIL...>::Inner;

      struct Inner : Next {
         using Next::cache;

         auto get(LoopSeq seq, LoopActionType& v) -> SchedAction* {
            if(seq == V_SEQ) {
               v = T_TRAITS<T_HEAD>::ActionType;
               return &action;
            } else {
               return Next::get(seq, v);
            }
         }

      private:
         Action action;
      };
   };

   template<typename T, typename T_SIGNATURE>
   using IsLoopPred = typename std::enable_if_t<std::is_base_of_v<T_SIGNATURE, T> && (sizeof(T) > 1)>;

   template<typename T_SIGNATURE, LoopActionType V_ACTION_TYPE, typename T_ACTION>
   struct LoopPredTraits {
      using Action = typename LoopPredAction<T_SIGNATURE>::template GenericAction<T_ACTION>;
      constexpr static LoopActionType ActionType = V_ACTION_TYPE;
   };

   /////////////////////////////////////////////////////////////////////////////////////////
   template<typename T>
   using BreakTraits = LoopPredTraits<BreakSignature, LoopActionType::BREAK_PRED, T>;

   template<size_t V_SIZE, size_t V_ALIGN, LoopSeq V_SEQ, typename T_HEAD, typename ... T_TAIL>
   struct GenericLoop_<V_SIZE, V_ALIGN, V_SEQ, IsLoopPred<T_HEAD, BreakSignature>, T_HEAD, T_TAIL...>
      : GenericLoopNonEmpty_<BreakTraits, V_SIZE, V_ALIGN, V_SEQ, T_HEAD, T_TAIL...>
   {};

   /////////////////////////////////////////////////////////////////////////////////////////
   template<typename T>
   using ContinueTraits = LoopPredTraits<ContinueSignature, LoopActionType::CONTINUE_PRED, T>;

   template<size_t V_SIZE, size_t V_ALIGN, LoopSeq V_SEQ, typename T_HEAD, typename ... T_TAIL>
   struct GenericLoop_<V_SIZE, V_ALIGN, V_SEQ, IsLoopPred<T_HEAD, ContinueSignature>, T_HEAD, T_TAIL...>
      : GenericLoopNonEmpty_<ContinueTraits, V_SIZE, V_ALIGN, V_SEQ, T_HEAD, T_TAIL...>
   {};

   ///////////////////////////////////////////////////////////////////////////////////////
   template<
      template<typename T> typename T_TRAITS,
      size_t V_SIZE,
      size_t V_ALIGN,
      LoopSeq V_SEQ,
      typename T_HEAD,
      typename ... T_TAIL>
   struct GenericLoopEmpty_ {
      using Action = typename T_TRAITS<T_HEAD>::Action;
      using Next =
      typename GenericLoop_<
         std::max(V_SIZE, sizeof(Action)),
         std::max(V_ALIGN, alignof(Action)),
         V_SEQ + 1,
         void,
         T_TAIL...>::Inner;

      struct Inner : Next {
         using Next::cache;

         auto get(LoopSeq seq, LoopActionType& v) -> SchedAction* {
            if(seq == V_SEQ) {
               v = T_TRAITS<T_HEAD>::ActionType;
               return new (cache) Action;
            } else {
               return Next::get(seq, v);
            }
         }
      };
   };

   ///////////////////////////////////////////////////////////////////////////////////////
   template<typename T, typename T_SIGNATURE>
   using IsEmptyLoopPred =
   typename std::enable_if_t<std::is_base_of_v<T_SIGNATURE, T> && \
         sizeof(T) == 1>;

   template<size_t V_SIZE, size_t V_ALIGN, LoopSeq V_SEQ, typename T_HEAD, typename ... T_TAIL>
   struct GenericLoop_<V_SIZE, V_ALIGN, V_SEQ, IsEmptyLoopPred<T_HEAD, BreakSignature>, T_HEAD, T_TAIL...>
      : GenericLoopEmpty_<BreakTraits, V_SIZE, V_ALIGN, V_SEQ, T_HEAD, T_TAIL...>{};

   template<size_t V_SIZE, size_t V_ALIGN, LoopSeq V_SEQ, typename T_HEAD, typename ... T_TAIL>
   struct GenericLoop_<V_SIZE, V_ALIGN, V_SEQ, IsEmptyLoopPred<T_HEAD, ContinueSignature>, T_HEAD, T_TAIL...>
      : GenericLoopEmpty_<ContinueTraits, V_SIZE, V_ALIGN, V_SEQ, T_HEAD, T_TAIL...> {};

   ///////////////////////////////////////////////////////////////////////////////////////
   template<typename T>
   struct ActionTraits {
      using Action = T;
      constexpr static LoopActionType ActionType = LoopActionType::ACTION;
   };

   template<size_t V_SIZE, size_t V_ALIGN, LoopSeq V_SEQ, typename T_HEAD, typename ... T_TAIL>
   struct GenericLoop_<V_SIZE, V_ALIGN, V_SEQ, IsSchedAction<T_HEAD>, T_HEAD, T_TAIL...>
      : GenericLoopEmpty_<ActionTraits, V_SIZE, V_ALIGN, V_SEQ, T_HEAD, T_TAIL...>{};

   /////////////////////////////////////////////////////////////////////////////////////////////
   template<size_t T_SIZE, size_t T_ALIGN, LoopSeq T_SEQ>
   struct GenericLoop_<T_SIZE, T_ALIGN, T_SEQ> {
      struct Inner  {
         auto get(LoopSeq seq, LoopActionType&) -> SchedAction* {
            return nullptr;
         }
      protected:
         alignas(T_ALIGN) char cache[T_SIZE];
      };
   };

   template<uint32_t V_MAX_TIMES, typename T_ACTION, typename ... T_ACTIONS>
   struct LOOP__ {
      using Actions = typename GenericLoop_<0, 0, 0, void, T_ACTION, T_ACTIONS...>::Inner;
      struct Inner : private Actions, SchedLoop {
      private:
         OVERRIDE(getMaxTime() const -> uint32_t) {
            return V_MAX_TIMES;
         }

         OVERRIDE(getAction(uint16_t seq, LoopActionType& type) -> SchedAction*) {
            return Actions::get(seq, type);
         }
      };
   };
}

#define __loop(...) TSL_NS::details::LOOP__<1, __VA_ARGS__>::Inner
#define __loop_max(times, ...) TSL_NS::details::LOOP__<times, __VA_ARGS__>::Inner
#define __forever(...) TSL_NS::details::LOOP__<std::numeric_limits<uint32>::max(), __VA_ARGS__>::Inner

#define __break_if(pred, ...) decltype(TSL_NS::details::BreakPred::DeduceType<pred, ##__VA_ARGS__>())
#define __until(...) __break_if(__VA_ARGS__)
#define __redo_if(pred) decltype(TSL_NS::details::ContinuePred::DeduceType<pred, TSL_NS::Result::RESTART_REQUIRED>())
#define __while(pred, ...) __break_if(__not(pred), ##__VA_ARGS__)

TSL_NS_END

#endif //TRANS_DSL_2_LOOPHELPER_H
