//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_EVENTINFO_H
#define TRANS_DSL_2_EVENTINFO_H

#include <event/event_ns.h>
#include <cub/dci/Role.h>
#include <event/concept/EventId.h>
#include <event/Status.h>
#include <cstddef>

EV_NS_BEGIN

DEFINE_ROLE(EventInfo) {
   explicit EventInfo(const EventId eventId) : eventId(eventId) {}

   auto getEventId() const -> EventId {
      return eventId;
   }
   auto updateEventId(const EventId id) const -> void {
      this->eventId = id;
   }

   ABSTRACT(getMsg() const                     -> const void*);
   ABSTRACT(getMsgSize() const                 -> size_t);
   ABSTRACT(getSequenceNum() const             -> uint32_t);

private:
   mutable EventId      eventId;
};

EV_NS_END

#endif //TRANS_DSL_2_EVENTINFO_H
