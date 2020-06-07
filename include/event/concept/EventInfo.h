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
   ABSTRACT(getEventId() const                 -> EventId);
   ABSTRACT(getMsg() const                     -> const void*);
   ABSTRACT(getMsgSize() const                 -> size_t);
   ABSTRACT(updateEventId(const EventId) const -> Status);
};

EV_NS_END

#endif //TRANS_DSL_2_EVENTINFO_H
