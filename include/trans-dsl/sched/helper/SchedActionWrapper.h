//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_SCHEDACTIONWRAPPER_H
#define TRANS_DSL_2_SCHEDACTIONWRAPPER_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/concept/SchedAction.h>

TSL_NS_BEGIN

template <typename T_ACTION>
struct SchedActionWrapper : SchedAction {
   OVERRIDE(exec(TransactionContext& context) -> Status) {
      return action.exec(context);
   }

   OVERRIDE(handleEvent(TransactionContext& context, Event& event) -> Status) {
      return action.handleEvent(context, event);
   }

   OVERRIDE(stop(TransactionContext& context, Status cause) -> Status) {
      return action.stop(context, cause);
   }

   OVERRIDE(kill(TransactionContext& context, Status cause) -> void) {
      action.kill(context, cause);
   }

private:
   T_ACTION action;
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDACTIONWRAPPER_H
