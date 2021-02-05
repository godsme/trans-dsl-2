//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_EVENTHANDLERREGISTRY_H
#define TRANS_DSL_2_EVENTHANDLERREGISTRY_H

#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/domain/EventId.h>
#include <trans-dsl/action/DummyAsyncAction.h>
#include <trans-dsl/sched/domain/Event.h>
#include <cstddef>
#include <variant>

TSL_NS_BEGIN

// 16 bytes
struct EventHandlerRegistry {
   EventHandlerRegistry();

   auto handleEvent(
      TransactionInfo const& trans,
      Event const& event) -> Status;

   auto addHandler(EventId eventId, details::NormalFunction handler) -> Status;
   auto addHandler(EventId eventId, details::MemberFunction handler) -> Status;
   auto reset() -> void;
   auto isWaiting() const -> bool;
private:
   std::variant<std::monostate, details::MemberFunction, details::NormalFunction> handler;
   EventId eventId;
};

TSL_NS_END

#endif //TRANS_DSL_2_EVENTHANDLERREGISTRY_H
