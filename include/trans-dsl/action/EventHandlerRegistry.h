//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_EVENTHANDLERREGISTRY_H
#define TRANS_DSL_2_EVENTHANDLERREGISTRY_H

#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/concept/EventId.h>
#include <trans-dsl/action/DummyAsyncAction.h>
#include <cstddef>
#include <trans-dsl/sched/concept/Event.h>

TSL_NS_BEGIN

struct EventHandlerRegistry {
   EventHandlerRegistry();

   auto handleEvent(
      details::DummyAsyncAction* this__,
      const TransactionInfo& trans,
      const Event& event) -> Status;

   auto addHandler(EventId eventId, details::NormalFunction handler) -> Status;
   auto reset() -> void;

private:
   details::NormalFunction handler;
   EventId eventId;
};

TSL_NS_END

#endif //TRANS_DSL_2_EVENTHANDLERREGISTRY_H
