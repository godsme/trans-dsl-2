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
      NilEventInfo() : EventInfo(INVALID_EVENT_ID) {}

      OVERRIDE(getMsg() const -> void const*) {
         return 0;
      }

      OVERRIDE(getMsgSize() const -> size_t) {
         return 0;
      }

      OVERRIDE(getSequenceNum() const -> uint32_t) {
          return 0xFFFF'FFFF;
      }
   };
}

Event::Event() : info(&NilEventInfo::getInstance()) {}

EV_NS_END
