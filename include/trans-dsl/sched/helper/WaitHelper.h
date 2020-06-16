//
// Created by Darwin Yuan on 2020/6/16.
//

#ifndef TRANS_DSL_2_WAITHELPER_H
#define TRANS_DSL_2_WAITHELPER_H

#include <trans-dsl/sched/action/SchedWait.h>

TSL_NS_BEGIN

namespace details {
   template<EV_NS::EventId V_EVENT_ID>
   struct Wait_ : SchedWait {
   private:
      OVERRIDE(getEventId() const -> EV_NS::EventId) {
         return V_EVENT_ID;
      }
   };
}

TSL_NS_END

#define __wait(eventId) TSL_NS::details::Wait_<eventId>

#endif //TRANS_DSL_2_WAITHELPER_H
