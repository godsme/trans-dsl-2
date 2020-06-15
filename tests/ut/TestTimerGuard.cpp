//
// Created by Darwin Yuan on 2020/6/15.
//

#include <cctest/cctest.h>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/TimerGuardHelper.h>

namespace {
   using namespace TSL_NS;

   FIXTURE(TestTimerGuard) {
     __timer_guard(TIMER_1, __async(AsyncAction1)) action;

      StupidTransactionContext context{};

      const EV_NS::SimpleEventInfo timerEventInfo{TIMER_EVENT_ID_1};
      const EV_NS::Event timerEvent{timerEventInfo};

     TEST("exec should return TIMEDOUT") {
        ASSERT_EQ(Result::CONTINUE, action.exec(context));
        ASSERT_EQ(Result::TIMEDOUT, action.handleEvent(context, timerEvent));
     }
   };

}