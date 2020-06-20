//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_SYNCACTIONHELPER_H
#define TRANS_DSL_2_SYNCACTIONHELPER_H

#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/sched/action/SchedSyncAction.h>
#include <trans-dsl/sched/concepts/ConceptHelper.h>

TSL_NS_BEGIN

struct TransactionInfo;

namespace details {
   template<typename T>
   DEF_CONCEPT(ClassConcept, std::is_class_v<T>);

   /////////////////////////////////////////////////////////////////////////
   template<typename T_ACTION, size_t V_SIZE=sizeof(T_ACTION) VOID_CONCEPT>
   struct SyncAction;

   template<CONCEPT_C(ClassConcept, T_ACTION), size_t V_SIZE>
   struct SyncAction<T_ACTION, V_SIZE ENABLE_C(ClassConcept, T_ACTION)> : SchedSyncAction {
      OVERRIDE(exec(TransactionContext & context)->Status) {
         return check(action(context));
      }

   private:
      T_ACTION action;
   };

   template<CONCEPT_C(ClassConcept, T_ACTION)>
   struct SyncAction<T_ACTION, 1 ENABLE_C(ClassConcept, T_ACTION)> : SchedSyncAction {
      OVERRIDE(exec(TransactionContext & context)->Status) {
         return check(T_ACTION{}(context));
      }
   };

   /////////////////////////////////////////////////////////////////////////
   using SyncActionFunc = Status (*)(const TransactionInfo &);

   template<SyncActionFunc V_ACTION>
   struct CallAction : SchedSyncAction {
      OVERRIDE(exec(TransactionContext & context)->Status) {
         return check(V_ACTION(context));
      }
   };

   /////////////////////////////////////////////////////////////////////////
   template<typename T>
   auto deductSyncActionClass() -> SyncAction<T>;

   template<SyncActionFunc V_FUNC>
   auto deductSyncActionClass() -> CallAction<V_FUNC>;
}

#define __sync(M_action) decltype(TSL_NS::details::deductSyncActionClass<M_action>())

TSL_NS_END

#endif //TRANS_DSL_2_SYNCACTIONHELPER_H
