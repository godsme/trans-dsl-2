//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_SIMPLEASYNCACTION_H
#define TRANS_DSL_2_SIMPLEASYNCACTION_H

#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/concept/EventId.h>
#include <trans-dsl/action/DummyAsyncAction.h>

TSL_NS_BEGIN

struct TransactionInfo;
struct Event;

template<typename T_REGISTRY>
struct SimpleAsyncAction {
   auto handleEvent(const TransactionInfo& trans, const Event& event) -> Status {
      return registry.handleEvent(reinterpret_cast<details::DummyAsyncAction*>(this), trans, event);
   }

   auto kill(const TransactionInfo& trans, Status cause) {
      registry.reset();
   }

protected:
   template<typename T>
   auto waitOn(T* thisPointer, EventId eventId, Status (T::*handler)(const TransactionInfo&, const Event&)) -> Status {
      return registry.addHandler(
         eventId,
         reinterpret_cast<details::DummyEventHandler>(handler)
         );
   }

private:
   T_REGISTRY registry;
};

TSL_NS_END

#endif //TRANS_DSL_2_SIMPLEASYNCACTION_H
