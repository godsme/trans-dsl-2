//
// Created by Darwin Yuan on 2020/6/15.
//

#include <trans-dsl/porting/timer/PlatformSpecifiedTimer.h>
#include <trans-dsl/sched/concept/EventId.h>
#include <trans-dsl/sched/concept/Event.h>

TSL_NS_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace
{
   const EventId TIMER_EVENT_START = 400;
   const EventId TIMER_EVENT_END   = 430;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PlatformSpecificTimer::isTimerEvent(const ev::Event& event) const
{
   return event.getEventId() >= TIMER_EVENT_START && event.getEventId() < TIMER_EVENT_END;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
auto PlatformSpecificTimer::startTimer_(const TimerId, TimeLength) -> Status
{
   return Result::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlatformSpecificTimer::stopTimer_(const TimerId)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PlatformSpecificTimer::matches_(const Event& event, const TimerId timerId) const
{
   return timerId + TIMER_EVENT_START == event.getEventId();
}

TSL_NS_END