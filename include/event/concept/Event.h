//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_EVENT_H
#define TRANS_DSL_2_EVENT_H

#include <event/concept/EventId.h>
#include <event/Status.h>
#include <cstddef>

EV_NS_BEGIN

struct EventInfo;

struct Event
{
   Event() = default;
   Event(const EventInfo& info);

   auto getEventId() const -> EventId ;
   auto getMsg() const     -> const void*;
   auto getMsgSize() const -> size_t;

   auto matches(const EventId eventId) const -> bool;

   auto updateEventId(const EventId) const -> Status;
   auto assignEventInfoTo(Event&) const -> void;
   auto getEventInfo() const -> const EventInfo&
   {
      return *info;
   }

   auto consume() const    -> void;
   auto isConsumed() const -> bool;

private:
   const EventInfo* info = nullptr;
   mutable bool consumed = false;
};

EV_NS_END

#endif //TRANS_DSL_2_EVENT_H
