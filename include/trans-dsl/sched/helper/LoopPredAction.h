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
   struct GenericLoopAction final : private T_PRED, LoopPredActionBase {
      OVERRIDE(exec(TransactionContext& context) -> Status) {
         return getFinalResult(T_PRED::operator()(context));
      }

   private:
      auto getFinalResult(bool satisfied) const -> Status {
         return satisfied ? T_PRED::FinalResult : Result::MOVE_ON;
      }
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_LOOPPREDACTION_H
