//
// Created by Darwin Yuan on 2020/6/12.
//

#ifndef TRANS_DSL_2_SCHEDLOOP_H
#define TRANS_DSL_2_SCHEDLOOP_H

#include <trans-dsl/sched/concept/SchedAction.h>
#include <trans-dsl/sched/concept/RuntimeContext.h>

TSL_NS_BEGIN

struct TransactionContext;

enum class LoopActionType {
   ACTION,
   BREAK_PRED,
   CONTINUE_PRED
};

struct SchedLoop
   : private RuntimeContext
   , SchedAction {

   OVERRIDE(exec(TransactionContext& ) -> Status);
   OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status);
   OVERRIDE(stop(TransactionContext& ) -> Status);
   OVERRIDE(kill(TransactionContext& ) -> void);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDLOOP_H
