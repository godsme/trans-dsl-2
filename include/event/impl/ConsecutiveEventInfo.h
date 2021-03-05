//
// Created by Darwin Yuan on 2020/6/10.
//

#ifndef TRANS_DSL_2_CONSECUTIVEEVENTINFO_H
#define TRANS_DSL_2_CONSECUTIVEEVENTINFO_H

#include <event/concept/EventInfo.h>

EV_NS_BEGIN

struct ConsecutiveEventInfo : EventInfo
{
   ConsecutiveEventInfo(const EventId eventId, uint32_t seq, const void* msg, size_t size)
            : EventInfo(eventId), msg(msg), size(size), seq(seq)
   {}

   ConsecutiveEventInfo(const EventId eventId, const void* msg, size_t size)
      : ConsecutiveEventInfo(eventId, 0xFFFF'FFFF, msg, size)
   {}

   template <typename T>
   ConsecutiveEventInfo(const EventId eventId, uint32_t seq, const T& msg)
      : ConsecutiveEventInfo(eventId, seq, reinterpret_cast<const void*>(&msg), sizeof(msg))
   {}

   template <typename T>
   ConsecutiveEventInfo(const EventId eventId, const T& msg)
      : ConsecutiveEventInfo(eventId, reinterpret_cast<const void*>(&msg), sizeof(msg))
   {}

   OVERRIDE(getMsg() const -> const void*) {
      return msg;
   }

   OVERRIDE(getMsgSize() const -> size_t) {
      return size;
   }

   OVERRIDE(getSequenceNum() const -> uint32_t) {
      return seq;
   }

private:
   const void* const  msg;
   const size_t       size;
   const uint32_t     seq;
};

EV_NS_END

#endif //TRANS_DSL_2_CONSECUTIVEEVENTINFO_H
