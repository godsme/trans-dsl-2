//
// Created by Darwin Yuan on 2020/6/12.
//

#ifndef TRANS_DSL_2_LOOPPREDACTION_H
#define TRANS_DSL_2_LOOPPREDACTION_H

#include <trans-dsl/sched/helper/Pred.h>
#include <trans-dsl/sched/helper/LoopPred.h>

TSL_NS_BEGIN

namespace details {

   struct LoopPredActionBase : SchedAction {
      OVERRIDE(handleEvent(TransactionContext&, Event const&) -> Status) {
         return Result::FATAL_BUG;
      }
      OVERRIDE(stop(TransactionContext&, Status) -> Status) {
         return Result::FATAL_BUG;
      }
      OVERRIDE(kill(TransactionContext&, Status) -> void) {}
   };

   template<typename T_PRED>
   struct LoopPredAction : LoopPredActionBase {
   protected:
      auto getFinalResult(bool satisfied) const -> Status {
         return satisfied ? Status(T_PRED::FinalResult) : Status(Result::MOVE_ON);
      }
   };

   template<typename T_BREAK, size_t V_SIZE = sizeof(T_BREAK)>
   struct GenericLoopAction : LoopPredAction<T_BREAK> {
      OVERRIDE(exec(TransactionContext& context) -> Status) {
         return LoopPredAction<T_BREAK>::getFinalResult(breakPred(context));
      }

   private:
      T_BREAK breakPred;
   };

   template<typename T_BREAK>
   struct GenericLoopAction<T_BREAK, 1> : LoopPredAction<T_BREAK> {
      OVERRIDE(exec(TransactionContext& context) -> Status) {
         return LoopPredAction<T_BREAK>::getFinalResult(T_BREAK{}(context));
      }
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_LOOPPREDACTION_H
