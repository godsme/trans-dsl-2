//
// Created by Darwin Yuan on 2020/6/15.
//

#include <trans-dsl/sched/concept/RelativeTimer.h>
#include <trans-dsl/sched/concept/TimerInfo.h>

TSL_NS_BEGIN

/////////////////////////////////////////////////////////////////
RelativeTimer::RelativeTimer(TimerId timerId)
   : timerId(timerId)
{}

/////////////////////////////////////////////////////////////////
Status RelativeTimer::start(const TimerInfo& info) {
   stop();

   TimeLength len = info.getTimeLength(timerId);
   if(len <= 0) {
      return Result::FAILED;
   }

   Status status = startTimer_(timerId, len);
   if(status != Result::SUCCESS) {
      return status;
   }

   started = true;

   return Result::SUCCESS;
}

/////////////////////////////////////////////////////////////////
void RelativeTimer::stop() {
   if(!started) return;
   stopTimer_(timerId);
}

/////////////////////////////////////////////////////////////////
bool RelativeTimer::matches(const ev::Event& event) const {
   if(!started || !isTimerEvent(event)) return false;
   return matches_(event, timerId);
}

TSL_NS_END
