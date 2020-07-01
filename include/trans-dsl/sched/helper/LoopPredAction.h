//
// Created by Darwin Yuan on 2020/6/12.
//

#ifndef TRANS_DSL_2_LOOPPREDACTION_H
#define TRANS_DSL_2_LOOPPREDACTION_H

#include <trans-dsl/sched/helper/Pred.h>
#include <trans-dsl/sched/helper/LoopPred.h>
#include <trans-dsl/sched/action/SchedSyncAction.h>

TSL_NS_BEGIN

namespace details {
   template<typename T_PRED>
   struct GenericLoopPredAction final : private T_PRED, SchedSyncAction {
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
