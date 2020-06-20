//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_ASYNCACTIONHELPER_H
#define TRANS_DSL_2_ASYNCACTIONHELPER_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/sched/domain/SchedAction.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/sched/concepts/ActionConcept.h>

TSL_NS_BEGIN

namespace details {

#if __CONCEPT_ENABLED
   template<ActionConcept T_ACTION>
#else
   template<typename T_ACTION>
#endif

   struct ASYNC__ : SchedAction {
      OVERRIDE(exec(TransactionContext & context)->Status) {
         return action.exec(context);
      }

      OVERRIDE(handleEvent(TransactionContext & context, const Event &event) -> Status) {
         return action.handleEvent(context, event);
      }

      OVERRIDE(stop(TransactionContext & context, Status cause)->Status) {
         action.kill(context, cause);
         return Result::FORCE_STOPPED;
      }

      OVERRIDE(kill(TransactionContext & context, Status cause)-> void) {
         action.kill(context, cause);
      }

   private:
      T_ACTION action;
   };
}

#define __asyn(action) TSL_NS::details::ASYNC__<action>

TSL_NS_END

#endif //TRANS_DSL_2_ASYNCACTIONHELPER_H
