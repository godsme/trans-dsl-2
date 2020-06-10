//
// Created by Darwin Yuan on 2020/6/10.
//

#include <cctest/cctest.h>
#include <event/impl/ConsecutiveEventInfo.h>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/SequentialHelper.h>
#include <trans-dsl/sched/helper/SyncActionHelper.h>

namespace {
   using namespace TSL_NS;

   FIXTURE(TestSequentialAction) {
      __sequential(
         __call(SyncAction1),
         __async(AsyncAction1),
         __sync(SyncAction3),
         __async(AsyncAction2),
         __call(SyncAction2)
         ) action;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      TEST("exec should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
      }

      TEST("handleEvent(event1) should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event1));
      }

      TEST("handleEvent(event2) should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::UNKNOWN_EVENT, action.handleEvent(context, event2));
      }

      TEST("handleEvent(event1) -> handleEvent(event2) should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event1));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event2));
      }

      TEST("handleEvent(event1) -> stop should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event1));
         ASSERT_EQ(Result::SUCCESS, action.stop(context, Result::FORCE_STOPPED));
      }

      TEST("after stop, handleEvent should return UNKNOWN_EVENT") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event1));
         ASSERT_EQ(Result::SUCCESS, action.stop(context, Result::FORCE_STOPPED));
         ASSERT_EQ(Result::UNKNOWN_EVENT, action.handleEvent(context, event2));
      }

      TEST("after kill, handleEvent should return UNKNOWN_EVENT") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event1));
         action.kill(context, Result::FORCE_STOPPED);
         ASSERT_EQ(Result::UNKNOWN_EVENT, action.handleEvent(context, event2));
      }

      TEST("after kill, handleEvent should return UNKNOWN_EVENT") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         action.kill(context, Result::FORCE_STOPPED);
         ASSERT_EQ(Result::UNKNOWN_EVENT, action.handleEvent(context, event1));
      }
   };
}