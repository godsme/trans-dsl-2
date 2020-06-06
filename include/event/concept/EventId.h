//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_EVENTID_H
#define TRANS_DSL_2_EVENTID_H

#include <event/event_ns.h>
#include <cstdint>

EV_NS_BEGIN

using EventId = uint16_t;

const EventId INVALID_EVENT_ID = 0xFFFF;

constexpr bool is_invalid_event(EventId eventId) {
   return eventId == INVALID_EVENT_ID;
}

EV_NS_END

#endif //TRANS_DSL_2_EVENTID_H
