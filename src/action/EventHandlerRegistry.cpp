//
// Created by Darwin Yuan on 2020/6/9.
//
#include <trans-dsl/action/EventHandlerRegistry.h>
#include <event/concept/Event.h>

TSL_NS_BEGIN

EventHandlerRegistry::EventHandlerRegistry() {
   reset();
}

auto EventHandlerRegistry::handleEvent(details::DummyAsyncAction* this__, TransactionInfo const& trans, Event const& event) -> Status {
   if(this->handler == nullptr) {
      return USER_FATAL_BUG;
   }

   if(this->eventId == event.getEventId()) {
      event.consume();
      auto result = handler(this__, trans, event);
      reset();
      return result;
   }

   return Result::UNKNOWN_EVENT;
}

auto EventHandlerRegistry::addHandler(EventId eventId, details::NormalFunction handler) -> Status {
   if(this->handler != nullptr) {
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