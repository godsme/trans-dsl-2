//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_SYNCACTIONHELPER_H
#define TRANS_DSL_2_SYNCACTIONHELPER_H

#include <trans-dsl/sched/concept/TransactionContext.h>
#include <trans-dsl/sched/action/SchedSyncAction.h>
#include <cub/base/IsClass.h>

TSL_NS_BEGIN

struct TransactionInfo;

namespace details {
   /////////////////////////////////////////////////////////////////////////
   template<typename T_ACTION, size_t V_SIZE=sizeof(T_ACTION), typename = void>
   struct SyncAction;

   template<typename T_ACTION, size_t V_SIZE>
   struct SyncAction<T_ACTION, V_SIZE, CUB_NS::IsClass<T_ACTION>> : SchedSyncAction {
      OVERRIDE(exec(TransactionContext & context)->Status) {
         return check(action(context));
      }

   private:
      T_ACTION action;
   };

   template<typename T_ACTION>
   struct SyncAction<T_ACTION, 1, CUB_NS::IsClass<T_ACTION>> : SchedSyncAction {
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
#define __call(...) __sync(__VA_ARGS__)

TSL_NS_END

#endif //TRANS_DSL_2_SYNCACTIONHELPER_H
