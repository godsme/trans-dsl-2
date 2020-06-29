//
// Created by Darwin Yuan on 2020/6/15.
//

#include <cctest/cctest.h>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/TimeGuardHelper.h>
#include <trans-dsl/sched/helper/ProcedureHelper.h>
#include <trans-dsl/sched/helper/SyncActionHelper.h>

namespace {
   using namespace TSL_NS;

   FIXTURE(TestTimerGuard) {
     __time_guard(TIMER_1, __asyn(AsyncAction1)) action;

      StupidTransactionContext context{};

      const EV_NS::SimpleEventInfo timerEventInfo{TIMER_EVENT_ID_1};
      const EV_NS::Event timerEvent{timerEventInfo};

     TEST("handleEvent should return TIMEOUT") {
        ASSERT_EQ(Result::CONTINUE, action.exec(context));
        ASSERT_EQ(Result::TIMEOUT, action.handleEvent(context, timerEvent));
     }

     TEST("stop should return FORCE_STOPPED") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::FORCE_STOPPED, action.stop(context, Result::TIMEOUT));
     }
   };


   FIXTURE(TestTimerGuard2) {
      using ProcedureAction =
      __procedure(
         __sync(SyncAction2),
         __finally(__asyn(AsyncAction1)));

      __time_guard(TIMER_1, ProcedureAction) action;

      StupidTransactionContext context{};

      const EV_NS::SimpleEventInfo timerEventInfo{TIMER_EVENT_ID_1};
      const EV_NS::Event timerEvent{timerEventInfo};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      TEST("stop should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::TIMEOUT));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event1));
      }

      TEST("handleEvent should return TIMEOUT") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, timerEvent));
         ASSERT_EQ(Result::TIMEOUT, action.handleEvent(context, event1));
      }

      TEST("if a timeout event occurred, should return TIMEOUT") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::OUT_OF_SCOPE));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, timerEvent));
         ASSERT_EQ(Result::TIMEOUT, action.handleEvent(context, event1));
      }
   };

   FIXTURE(TestTimerGuard3) {
      using ProcedureAction =
      __procedure(
         __asyn(AsyncAction2),
         __finally(__asyn(AsyncAction1)));

      __time_guard(TIMER_1, ProcedureAction) action;

      StupidTransactionContext context{};

      const EV_NS::SimpleEventInfo timerEventInfo{TIMER_EVENT_ID_1};
      const EV_NS::Event timerEvent{timerEventInfo};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};


      TEST("stop should return FORCE_STOPPED") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::TIMEOUT));
         ASSERT_EQ(Result::FORCE_STOPPED, action.handleEvent(context, event1));
      }

      TEST("normally should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event2));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event1));
      }

      TEST("normally should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::UNKNOWN_EVENT, action.handleEvent(context, event1));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event2));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event1));
      }

      TEST("handleEvent should return TIMEOUT") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, timerEvent));
         ASSERT_EQ(Result::TIMEOUT, action.handleEvent(context, event1));
      }

      TEST("if a timeout event occurred, should return TIMEOUT") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::OUT_OF_SCOPE));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, timerEvent));
         ASSERT_EQ(Result::TIMEOUT, action.handleEvent(context, event1));
      }

      TEST("if a timeout event occurred, should return TIMEOUT") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event2));
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::OUT_OF_SCOPE));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, timerEvent));
         ASSERT_EQ(Result::TIMEOUT, action.handleEvent(context, event1));
      }

      TEST("if a timeout event occurred, should return TIMEOUT") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event2));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, timerEvent));
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::OUT_OF_SCOPE));
         ASSERT_EQ(Result::TIMEOUT, action.handleEvent(context, event1));
      }
   };
}