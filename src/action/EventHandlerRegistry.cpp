//
// Created by Darwin Yuan on 2020/6/9.
//
#include <trans-dsl/action/EventHandlerRegistry.h>
#include <event/concept/Event.h>

TSL_NS_BEGIN

EventHandlerRegistry::EventHandlerRegistry() {
   reset();
}

auto EventHandlerRegistry::handleEvent(TransactionInfo const& trans, Event const& event) -> Status {
   if(this->handler.index() == 0) {
      return USER_FATAL_BUG;
   }

   if(this->eventId == event.getEventId()) {
      event.consume();
      Status status;
      if(this->handler.index() == 1) {
         status = std::get<1>(handler)(nullptr, trans, *reinterpret_cast<details::DummyMsgType const*>(event.getMsg()));
      } else {
         status = std::get<2>(handler)(trans, *reinterpret_cast<details::DummyMsgType const*>(event.getMsg()));
      }

      reset();
      return status;
   }

   return Result::UNKNOWN_EVENT;
}

auto EventHandlerRegistry::addHandler(EventId eventId, details::NormalFunction handler) -> Status {
   if(this->handler.index() != 0) {
      return Result::OUT_OF_SCOPE;
   }

   this->handler = handler;
   this->eventId = eventId;

   return Result::CONTINUE;
}

auto EventHandlerRegistry::addHandler(EventId eventId, details::MemberFunction handler) -> Status {
    if(this->handler.index() != 0) {
       return Result::OUT_OF_SCOPE;
    }

    this->handler = handler;
    this->eventId = eventId;

    return Result::CONTINUE;
 }

auto EventHandlerRegistry::isWaiting() const -> bool {
   return handler.index() != 0;
}

auto EventHandlerRegistry::reset() -> void {
   this->handler = std::monostate{};
   this->eventId = INVALID_EVENT_ID;
}

TSL_NS_END