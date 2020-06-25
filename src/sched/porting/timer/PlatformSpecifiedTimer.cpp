//
// Created by Darwin Yuan on 2020/6/15.
//

#include <trans-dsl/porting/timer/PlatformSpecifiedTimer.h>
#include <trans-dsl/sched/domain/EventId.h>
#include <trans-dsl/sched/domain/Event.h>

TSL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace
{
   const EventId TIMER_EVENT_START = 400;
   const EventId TIMER_EVENT_END   = 430;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PlatformSpecifiedTimer::isTimerEvent(Event const& event) const
{
   return event.getEventId() >= TIMER_EVENT_START && event.getEventId() < TIMER_EVENT_END;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
auto PlatformSpecifiedTimer::startTimer_(const TimerId, TimeLength) -> Status
{
   return Result::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlatformSpecifiedTimer::stopTimer_(const TimerId)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PlatformSpecifiedTimer::matches_(Event const& event, TimerId timerId) const
{
   return timerId + TIMER_EVENT_START == event.getEventId();
}

TSL_NS_END