//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_SYNCACTIONHELPER_H
#define TRANS_DSL_2_SYNCACTIONHELPER_H

#include <trans-dsl/sched/concept/TransactionContext.h>
#include <trans-dsl/sched/action/SchedSyncAction.h>

TSL_NS_BEGIN

struct TransactionInfo;

namespace details {
   template<typename T_ACTION>
   struct SYNC__ : SchedSyncAction {
      OVERRIDE(exec(TransactionContext & context)->Status) {
         return check(action.exec(context.ROLE(TransactionInfo)));
      }

   private:
      T_ACTION action;
   };

   using SyncActionFunc = Status (*)(const TransactionInfo &);

   template<SyncActionFunc V_ACTION>
   struct CALL__ : SchedSyncAction {
      OVERRIDE(exec(TransactionContext & context)->Status) {
         return check(V_ACTION(context.ROLE(TransactionInfo)));
      }
   };
}

#define __sync(action) TSL_NS::details::SYNC__<action>
#define __call(...) TSL_NS::details::CALL__<__VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_SYNCACTIONHELPER_H
