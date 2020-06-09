//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_ASYNCACTIONHELPER_H
#define TRANS_DSL_2_ASYNCACTIONHELPER_H

#include <trans-dsl/sched/concept/SchedAction.h>
#include <trans-dsl/sched/concept/TransactionContext.h>

TSL_NS_BEGIN

template<typename T_ACTION>
struct ASYNC__ : SchedAction {
   OVERRIDE(exec(TransactionContext& context) -> Status) {
      return action.handleEvent(context.ROLE(TransactionInfo));
   }
   OVERRIDE(handleEvent(TransactionContext& context, Event& event) -> Status) {
      return action.handleEvent(context.ROLE(TransactionInfo), event);
   }
   OVERRIDE(stop(TransactionContext& context, Status cause)  -> Status) {
      action.kill(context.ROLE(TransactionInfo), cause);
      return Result::SUCCESS;
   }
   OVERRIDE(kill(TransactionContext& context, Status cause)        -> void) {
      action.kill(context.ROLE(TransactionInfo), cause);
   }
private:
   T_ACTION action;
};

#define __async(action) TSL_NS::ASYNC__<action>

TSL_NS_END

#endif //TRANS_DSL_2_ASYNCACTIONHELPER_H
