//
// Created by Darwin Yuan on 2020/6/10.
//

#ifndef TRANS_DSL_2_CONSECUTIVEEVENTINFO_H
#define TRANS_DSL_2_CONSECUTIVEEVENTINFO_H

#include <event/concept/EventInfo.h>

EV_NS_BEGIN

struct ConsecutiveEventInfo : EventInfo
{
   ConsecutiveEventInfo(const EventId eventId, const void* const msg, size_t size)
      : EventInfo(eventId), msg(msg), size(size)
   {}

   template <typename T>
   ConsecutiveEventInfo(const EventId eventId, const T& msg)
      : ConsecutiveEventInfo(eventId, (void*)&msg, sizeof(msg))
   {}

   OVERRIDE(getMsg() const -> const void*) {
      return msg;
   }

   OVERRIDE(getMsgSize() const -> size_t) {
      return size;
   }

private:
   const void* const  msg;
   const size_t       size;
};

EV_NS_END

#endif //TRANS_DSL_2_CONSECUTIVEEVENTINFO_H
