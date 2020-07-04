//
// Created by Darwin Yuan on 2020/7/4.
//

#ifndef TRANS_DSL_2_TRANSACTION_H
#define TRANS_DSL_2_TRANSACTION_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/domain/MultiThreadContext.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <cstddef>
#include <trans-dsl/sched/domain/Event.h>
#include <trans-dsl/utils/ThreadActionTrait.h>

TSL_NS_BEGIN

template<typename SCHED>
struct Transaction : private TransactionContext, private SCHED {
   using TransactionContext::updateInstanceId;
   using TransactionContext::updateUserContext;
   using TransactionContext::updateTimerInfo;
   using TransactionContext::updateListener;

   auto start() -> Status {
      return SCHED::start(*this);
   }

   auto startWithEvent(Event const& event) -> Status {
      Status status = SCHED::start(*this);
      if(status == Result::CONTINUE) {
         return SCHED::handleEvent(*this, event);
      }
   }

   auto handleEvent(Event const& event) -> Status {
      return SCHED::handleEvent(*this, event);
   }

   auto stop(Status cause) -> Status {
      return SCHED::stop(*this, cause);
   }

   auto kill(Status cause) -> void {
      SCHED::kill(*this, cause);
   }
};

TSL_NS_END

#endif //TRANS_DSL_2_TRANSACTION_H
