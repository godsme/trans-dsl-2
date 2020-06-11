//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_LOOPHELPER_H
#define TRANS_DSL_2_LOOPHELPER_H

#include <trans-dsl/sched/concept/TransactionContext.h>
#include <trans-dsl/sched/action/SchedOptional.h>
#include <type_traits>
#include <cub/base/IsClass.h>
#include <algorithm>
#include <trans-dsl/sched/helper/Pred.h>

TSL_NS_BEGIN

namespace details {

   template<typename T>
   using IsSchedAction = std::enable_if_t<std::is_base_of_v<SchedAction, T>>;

   const uint64_t breakSignature = 0x9d3d'ae93'2cde'0924;

   template<typename T, Status V_RESULT = Result::UNSPECIFIED, size_t V_SIZE = sizeof(T), typename = void>
   struct BreakAction;

   struct BreakActionSignature {};
   template<Status V_RESULT>
   struct BreakActionBase : BreakActionSignature {
      enum : Status {
         FinalResult = V_RESULT
      };
   };

   template<typename T, Status V_RESULT, size_t V_SIZE>
   struct BreakAction<T, V_RESULT, V_SIZE, CUB_NS::IsClass<T>>
      : BreakActionBase<V_RESULT> {
      auto operator()(const TransactionInfo& context) -> bool {
         return pred(context);
      }

   private:
      T pred;
   };

   template<typename T, Status V_RESULT>
   struct BreakAction<T, V_RESULT, 1, CUB_NS::IsClass<T>>
      : BreakActionBase<V_RESULT> {
      auto operator()(const TransactionInfo& context) -> bool {
         return T{}(context);
      }
   };

   template<PredFunction V_FUNC, Status V_RESULT>
   struct BreakFunc : BreakActionBase<V_RESULT> {
      auto operator()(const TransactionInfo& context) -> bool {
         return V_FUNC(context);
      }
   };

   template<typename T, Status V_RESULT = Result::UNSPECIFIED>
   auto DeduceBreakType() -> BreakAction<T, V_RESULT>;

   template<PredFunction V_FUNC, Status V_RESULT = Result::UNSPECIFIED>
   auto DeduceBreakType() -> BreakFunc<V_FUNC, V_RESULT>;


   struct SchedBreakAction : SchedAction {
      OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status) {
         return Result::FATAL_BUG;
      }
      OVERRIDE(stop(TransactionContext&)                -> Status) {
         return Result::FATAL_BUG;
      }
      OVERRIDE(kill(TransactionContext&)                -> void) {}
   };

   template<typename T_BREAK>
   struct MiddleSchedBreakAction : SchedBreakAction {
   protected:
      auto getFinalResult(bool satisfied) const -> Status {
         return satisfied ? T_BREAK::FinalResult : Result::UNSATISFIED;
      }
   };

   template<typename T_BREAK, size_t V_SIZE = sizeof(T_BREAK)>
   struct GenericSchedBreakAction : MiddleSchedBreakAction<T_BREAK> {
      OVERRIDE(exec(TransactionContext& context) -> Status) {
         return MiddleSchedBreakAction<T_BREAK>::getFinalResult(breakPred(context));
      }

   private:
      T_BREAK breakPred;
   };

   template<typename T_BREAK>
   struct GenericSchedBreakAction<T_BREAK, 1> : MiddleSchedBreakAction<T_BREAK> {
      OVERRIDE(exec(TransactionContext& context) -> Status) {
         return MiddleSchedBreakAction<T_BREAK>::getFinalResult(T_BREAK{}(context.ROLE(TransactionInfo)));
      }
   };


   using LoopSeq = unsigned short;

   template<size_t V_SIZE, size_t V_ALIGN, LoopSeq V_SEQ, typename = void, typename ... T_ACTIONS>
   struct GenericLoop_;

   template<size_t V_SIZE, size_t V_ALIGN, LoopSeq V_SEQ, typename T_HEAD, typename ... T_TAIL>
   struct GenericLoop_<V_SIZE, V_ALIGN, V_SEQ, IsSchedAction<T_HEAD>, T_HEAD, T_TAIL...> {
      using Action = T_HEAD;
      using Next =
      typename GenericLoop_<
         std::max(V_SIZE, sizeof(Action)),
         std::max(V_ALIGN, alignof(Action)),
         V_SEQ + 1,
         void,
         T_TAIL...>::Inner;

      struct Inner : Next {
         using Next::cache;

         auto get(LoopSeq seq) -> SchedAction* {
            return seq == V_SEQ ? new (cache) Action : Next::get(seq);
         }
      };
   };

   template<typename T>
   using IsBreak = typename std::enable_if_t<std::is_base_of_v<BreakActionSignature, T>>;

   template<size_t V_SIZE, size_t V_ALIGN, LoopSeq V_SEQ, typename T_HEAD, typename ... T_TAIL>
   struct GenericLoop_<V_SIZE, V_ALIGN, V_SEQ, IsBreak<T_HEAD>, T_HEAD, T_TAIL...> {
      using Action = T_HEAD;
      using Next =
      typename GenericLoop_<
         V_SIZE,
         V_ALIGN,
         V_SEQ + 1,
         void,
         T_TAIL...>::Inner;

      struct Inner : Next {
         using Next::cache;

         auto get(LoopSeq seq) -> SchedAction* {
            return seq == V_SEQ ? &breakAction : Next::get(seq);
         }

      private:
         GenericSchedBreakAction<T_HEAD> breakAction;
      };
   };

   template<size_t T_SIZE, size_t T_ALIGN, LoopSeq T_SEQ>
   struct GenericLoop_<T_SIZE, T_ALIGN, T_SEQ> {
      struct Inner  {
         auto get(LoopSeq seq) -> SchedAction* {
            return nullptr;
         }
      protected:
         alignas(T_ALIGN) char cache[T_SIZE];
      };
   };

   template<typename T_ACTION, typename ... T_ACTIONS>
   struct LOOP__ {
      using Actions = typename GenericLoop_<0, 0, 0, void, T_ACTION, T_ACTIONS...>::Inner;
      struct Inner : Actions {
      };
   };
}

#define __loop(...) TSL_NS::details::LOOP__<__VA_ARGS__>::Inner
#define __break_if(pred, ...) decltype(TSL_NS::details::DeduceBreakType<pred, ##__VA_ARGS__>())

TSL_NS_END

#endif //TRANS_DSL_2_LOOPHELPER_H
