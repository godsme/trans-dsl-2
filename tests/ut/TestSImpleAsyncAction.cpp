//
// Created by Darwin Yuan on 2020/6/10.
//

#include <cctest/cctest.h>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"

namespace {
   using namespace TSL_NS;

   FIXTURE(TestSimpleAsyncAction) {
      AsyncAction1 action1;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      TEST("exec should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, action1.exec(context));
      }

      TEST("if exec is called again, it should return OUT_OF_SCOPE") {
         ASSERT_EQ(Result::CONTINUE, action1.exec(context));
         ASSERT_EQ(Result::OUT_OF_SCOPE, action1.exec(context));
      }

      TEST("handleEvent should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action1.exec(context));
         ASSERT_EQ(Result::SUCCESS, action1.handleEvent(context, event1));
      }

      TEST("if the EventId is not expected, handleEvent should return UNKNOWN_EVENT") {
         ASSERT_EQ(Result::CONTINUE, action1.exec(context));
         ASSERT_EQ(Result::UNKNOWN_EVENT, action1.handleEvent(context, Event(EV_NS::SimpleEventInfo(100))));
      }

      TEST("after event is accepted, event should be consumed") {
         ASSERT_EQ(Result::CONTINUE, action1.exec(context));

         ASSERT_FALSE(event1.isConsumed());
         ASSERT_EQ(Result::SUCCESS, action1.handleEvent(context, event1));
         ASSERT_TRUE(event1.isConsumed());
      }

      TEST("after an action has been done successfully, if passing an event with same type, should return USER_FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, action1.exec(context));
         ASSERT_EQ(Result::SUCCESS, action1.handleEvent(context, event1));
         ASSERT_EQ(Result::USER_FATAL_BUG, action1.handleEvent(context, event1));
      }

      TEST("when an action is waiting for an event, if it's killed, handleEvent will return USER_FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, action1.exec(context));
         action1.kill(context, Result::DUPTID);
         ASSERT_EQ(Result::USER_FATAL_BUG, action1.handleEvent(context, event1));
      }
   };
}
