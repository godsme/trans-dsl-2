//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_EVENT_H
#define TRANS_DSL_2_EVENT_H

#include <event/concept/EventId.h>
#include <event/Status.h>
#include <event/concept/EventInfo.h>
#include <cstddef>

EV_NS_BEGIN

struct EventInfo;

struct Event
{
   Event();
   Event(const EventInfo& info) : info(&info) {}

   auto getEventId() const -> EventId {
      return info->getEventId();
   }

   auto getMsg() const     -> const void* {
      return info->getMsg();
   }

   auto getMsgSize() const -> size_t {
      return info->getMsgSize();
   }

   auto matches(EventId eventId) const -> bool {
      return info->getEventId() == eventId;
   }

   auto updateEventId(EventId eventId) const -> void {
      info->updateEventId(eventId);
   }

   auto assignEventInfoTo(Event& another) const -> void {
      another.info = info;
   }

   auto getEventInfo() const -> const EventInfo&
   {
      return *info;
   }

   auto consume() const    -> void {
      consumed = true;
   }

   auto isConsumed() const -> bool {
      return consumed;
   }

private:
   const EventInfo* info = nullptr;
   mutable bool consumed = false;
};

EV_NS_END

#endif //TRANS_DSL_2_EVENT_H
