//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_ASYNCACTIONHELPER_H
#define TRANS_DSL_2_ASYNCACTIONHELPER_H

#include <trans-dsl/sched/concept/SchedAction.h>
#include <trans-dsl/sched/concept/TransactionContext.h>

TSL_NS_BEGIN

namespace details {
   template<typename T_ACTION>
   struct ASYNC__ : SchedAction {
      OVERRIDE(exec(TransactionContext & context)->Status) {
         return action.exec(context.ROLE(TransactionInfo));
      }

      OVERRIDE(handleEvent(TransactionContext & context, const Event &event) -> Status) {
         return action.handleEvent(context.ROLE(TransactionInfo), event);
      }

      OVERRIDE(stop(TransactionContext & context, Status cause)->Status) {
         action.kill(context.ROLE(TransactionInfo));
         return Result::FORCE_STOPPED;
      }

      OVERRIDE(kill(TransactionContext & context)-> void) {
         action.kill(context.ROLE(TransactionInfo));
      }

   private:
      T_ACTION action;
   };
}

#define __async(action) TSL_NS::details::ASYNC__<action>

TSL_NS_END

#endif //TRANS_DSL_2_ASYNCACTIONHELPER_H
