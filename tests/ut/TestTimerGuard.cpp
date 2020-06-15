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
   const TimerId TIMER_1 = 401;

   FIXTURE(TestTimerGuard) {
     __timer_guard(TIMER_1, __async(AsyncAction1)) action;

      StupidTransactionContext context{};
     TEST("exec should return CONTINUE") {
        ASSERT_EQ(Result::CONTINUE, action.exec(context));
     }
   };

}