//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_PLATFORMSPECIFIEDTIMER_H
#define TRANS_DSL_2_PLATFORMSPECIFIEDTIMER_H

#include <trans-dsl/sched/concept/RelativeTimer.h>

TSL_NS_BEGIN

struct PlatformSpecificTimer : RelativeTimer
{
   using RelativeTimer::RelativeTimer;

   OVERRIDE(isTimerEvent(const Event&) const -> bool);
   OVERRIDE(startTimer_(TimerId, TimeLength timerLen) -> Status);
   OVERRIDE(stopTimer_(TimerId) -> void);
   OVERRIDE(matches_(const Event&, TimerId) const -> bool);
};

TSL_NS_END

#endif //TRANS_DSL_2_PLATFORMSPECIFIEDTIMER_H
