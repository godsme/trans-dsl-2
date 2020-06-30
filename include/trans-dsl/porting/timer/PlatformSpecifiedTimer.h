//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_PLATFORMSPECIFIEDTIMER_H
#define TRANS_DSL_2_PLATFORMSPECIFIEDTIMER_H

#include <trans-dsl/sched/domain/RelativeTimer.h>

TSL_NS_BEGIN

struct PlatformSpecifiedTimer : RelativeTimer
{
   using RelativeTimer::RelativeTimer;

   OVERRIDE(startTimer_(TimerId, TimeLength timerLen) -> Status);
   OVERRIDE(stopTimer_(TimerId) -> void);
   OVERRIDE(matches_(Event const&, TimerId) const -> bool);

private:
   auto isTimerEvent(Event const&) const -> bool;
};

TSL_NS_END

#endif //TRANS_DSL_2_PLATFORMSPECIFIEDTIMER_H
