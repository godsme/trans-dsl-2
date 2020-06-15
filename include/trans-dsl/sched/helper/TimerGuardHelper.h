//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_TIMERGUARDHELPER_H
#define TRANS_DSL_2_TIMERGUARDHELPER_H

#include <trans-dsl/sched/action/SchedTimerGuard.h>
#include <trans-dsl/porting/timer/TimerId.h>
#include <trans-dsl/sched/helper/IsSchedAction.h>
#include <trans-dsl/porting/timer/PlatformSpecifiedTimer.h>

TSL_NS_BEGIN

namespace details {

   template<TimerId V_TIMER_ID, typename T_ACTION, typename = void>
   struct TimerGuard;

   template<TimerId V_TIMER_ID, typename T_ACTION>
   struct TimerGuard<V_TIMER_ID, T_ACTION, IsSchedAction<T_ACTION>> {
      struct Inner : SchedTimerGuard {
      private:
         IMPL_ROLE_WITH_VAR(SchedAction, action);
         IMPL_ROLE_WITH_VAR(RelativeTimer, timer);
      private:
         T_ACTION action;
         PlatformSpecificTimer timer{V_TIMER_ID};
      };
   };
}

TSL_NS_END

#define __timer_guard(timerId, ...) TSL_NS::details::TimerGuard<timerId, __VA_ARGS__>::Inner

#endif //TRANS_DSL_2_TIMERGUARDHELPER_H
