//
// Created by Darwin Yuan on 2020/6/9.
//

#include <event/event_ns.h>
#include <event/concept/Event.h>
#include <cub/gof/Singleton.h>
#include <event/concept/EventInfo.h>

EV_NS_BEGIN

namespace
{
   DEF_SINGLETON(NilEventInfo, EventInfo) {
      OVERRIDE(getEventId() const -> EventId) {
         return INVALID_EVENT_ID;
      }

      OVERRIDE(getMsg() const -> const void*) {
         return 0;
      }

      OVERRIDE(getMsgSize() const -> size_t) {
         return 0;
      }

      OVERRIDE(updateEventId(const EventId) const -> Status) {
         return CUB_NS::Result::FATAL_BUG;
      }
   };
}

Event::Event() : info(&NilEventInfo::getInstance()) {}

Event::Event(const EventInfo& info) : info(&info) {
}

auto Event::getEventId() const -> EventId {
   return info->getEventId();
}

auto Event::getMsg() const     -> const void* {
   return info->getMsg();
}

auto Event::getMsgSize() const -> size_t {
   return info->getMsgSize();
}

auto Event::matches(EventId eventId) const -> bool {
   return info->getEventId() == eventId;
}

auto Event::updateEventId(EventId eventId) const -> Status {
   return info->updateEventId(eventId);
}

auto Event::assignEventInfoTo(Event& another) const -> void {
   another.info = info;
}

EV_NS_END
