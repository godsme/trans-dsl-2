//
// Created by Darwin Yuan on 2020/6/12.
//

#ifndef TRANS_DSL_2_LOOPPREDACTION_H
#define TRANS_DSL_2_LOOPPREDACTION_H

#include <trans-dsl/sched/helper/Pred.h>
#include <trans-dsl/sched/helper/LoopPred.h>

TSL_NS_BEGIN

namespace details {

   template<typename SIGNATURE>
   struct LoopPredAction {
   private:
      struct Base : SchedAction {
         OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status) {
            return Result::FATAL_BUG;
         }
         OVERRIDE(stop(TransactionContext&, Status) -> Status) {
            return Result::FATAL_BUG;
         }
         OVERRIDE(kill(TransactionContext&, Status) -> void) {}
      };

      template<typename T_PRED>
      struct Action : Base {
      protected:
         auto getFinalResult(bool satisfied) const -> Status {
            return satisfied ? T_PRED::FinalResult : Result::MOVE_ON;
         }
      };

   public:
      template<typename T_BREAK, size_t V_SIZE = sizeof(T_BREAK)>
      struct GenericAction : Action<T_BREAK> {
         OVERRIDE(exec(TransactionContext& context) -> Status) {
            return Action<T_BREAK>::getFinalResult(breakPred(context));
         }

      private:
         T_BREAK breakPred;
      };

      template<typename T_BREAK>
      struct GenericAction<T_BREAK, 1> : Action<T_BREAK> {
         OVERRIDE(exec(TransactionContext& context) -> Status) {
            return Action<T_BREAK>::getFinalResult(T_BREAK{}(context));
         }
      };
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_LOOPPREDACTION_H
