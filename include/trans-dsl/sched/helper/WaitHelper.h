//
// Created by Darwin Yuan on 2020/6/16.
//

#ifndef TRANS_DSL_2_WAITHELPER_H
#define TRANS_DSL_2_WAITHELPER_H

#include <trans-dsl/sched/action/SchedWait.h>

TSL_NS_BEGIN

namespace details {
   template<bool V_IS_WAIT, EV_NS::EventId V_EVENT_ID>
   struct Wait_ final : SchedWait {
   private:
      OVERRIDE(getEventId() const -> EV_NS::EventId) { return V_EVENT_ID; }
      OVERRIDE(isWait() const -> bool) { return V_IS_WAIT; }
   };
}

TSL_NS_END

#define __wait(eventId) TSL_NS::details::Wait_<true, eventId>
#define __peek(eventId) TSL_NS::details::Wait_<false, eventId>

#endif //TRANS_DSL_2_WAITHELPER_H
