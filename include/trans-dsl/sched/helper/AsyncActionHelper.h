//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_ASYNCACTIONHELPER_H
#define TRANS_DSL_2_ASYNCACTIONHELPER_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/sched/domain/SchedAction.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/sched/concepts/ActionConcept.h>
#include <trans-dsl/sched/concepts/ConceptHelper.h>

TSL_NS_BEGIN

namespace details {

   template<CONCEPT(ActionConcept) T_ACTION>
   struct Async final : SchedAction {
      CONCEPT_ASSERT(ActionConcept<T_ACTION>);

      OVERRIDE(exec(TransactionContext & context)->Status) {
         return action.exec(context);
      }

      OVERRIDE(handleEvent(TransactionContext & context, Event const& event) -> Status) {
         return action.handleEvent(context, event);
      }

      OVERRIDE(stop(TransactionContext & context, Status cause)->Status) {
         Status status = action.kill(context, cause);
         return status == Result::SUCCESS ? Result::FORCE_STOPPED : Result::USER_FATAL_BUG;
      }

      OVERRIDE(kill(TransactionContext & context, Status cause)-> void) {
         (void) action.kill(context, cause);
      }

   private:
      T_ACTION action;
   };
}

#define __asyn(action) TSL_NS::details::Async<action>

TSL_NS_END

#endif //TRANS_DSL_2_ASYNCACTIONHELPER_H
