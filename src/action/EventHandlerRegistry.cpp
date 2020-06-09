//
// Created by Darwin Yuan on 2020/6/9.
//
#include <trans-dsl/action/EventHandlerRegistry.h>
#include <event/concept/Event.h>

TSL_NS_BEGIN

EventHandlerRegistry::EventHandlerRegistry() {
   reset();
}

auto EventHandlerRegistry::handleEvent(details::DummyAsyncAction* this__, const TransactionInfo& trans, const Event& event) -> Status {
   if(this->eventId == event.getEventId()) {
      event.consume();
      auto result = (this__->*handler)(trans, event);
      reset();
      return result;
   }

   return Result::UNKNOWN_EVENT;
}

auto EventHandlerRegistry::addHandler(EventId eventId, details::DummyEventHandler handler) -> Status {
   if(handler != nullptr) {
      return Result::OUT_OF_SCOPE;
   }

   this->handler = handler;
   this->eventId = eventId;

   return Result::CONTINUE;
}

auto EventHandlerRegistry::reset() -> void {
   this->handler = nullptr;
   this->eventId = INVALID_EVENT_ID;
}

TSL_NS_END