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

   template<typename T, std::enable_if_t<std::is_class_v<T>, int> = 0>
   auto DEDUCT_SYNC_TYPE__() { return SYNC__<T>{}; }

   template<SyncActionFunc V_FUNC>
   auto DEDUCT_SYNC_TYPE__() { return CALL__<V_FUNC>{}; }
}

#define __sync(action) decltype(TSL_NS::details::DEDUCT_SYNC_TYPE__<action>())
#define __call(...) __sync(__VA_ARGS__)

TSL_NS_END

#endif //TRANS_DSL_2_SYNCACTIONHELPER_H
