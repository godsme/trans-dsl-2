//
// Created by Darwin Yuan on 2020/6/15.
//

#include <cctest/cctest.h>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/SwitchCaseHelper.h>
#include <trans-dsl/sched/helper/ProcedureHelper.h>
#include <trans-dsl/sched/helper/OptionalHelper.h>
#include <iostream>

namespace {
   using namespace TSL_NS;

   FIXTURE(TestTimerGuard) {
      __switch(
         __case(__is_status(Result::OUT_OF_SCOPE), __asyn(AsyncAction1)),
         __case(__is_status(Result::INVALID_DATA), __asyn(AsyncAction2)),
         __otherwise(__asyn(AsyncAction4))) action;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      const Msg4 msg4{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo4{EV_MSG_4, msg4};
      TSL_NS::Event event4{eventInfo4};

      TEST("should choose path 1") {
         context.getRuntimeContext().reportFailure(Result::OUT_OF_SCOPE);
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event1));
      }

      TEST("should choose path 1") {
         context.getRuntimeContext().reportFailure(Result::OUT_OF_SCOPE);
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::UNKNOWN_EVENT, action.handleEvent(context, event2));
         ASSERT_EQ(Result::UNKNOWN_EVENT, action.handleEvent(context, event4));
      }

      TEST("should choose path 2") {
         context.getRuntimeContext().reportFailure(Result::INVALID_DATA);
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event2));
      }

      TEST("should choose path 2") {
         context.getRuntimeContext().reportFailure(Result::INVALID_DATA);
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::UNKNOWN_EVENT, action.handleEvent(context, event1));
         ASSERT_EQ(Result::UNKNOWN_EVENT, action.handleEvent(context, event4));
      }

      TEST("should choose path 3") {
         context.getRuntimeContext().reportFailure(Result::TIMEDOUT);
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event4));
      }

      TEST("should choose path 3") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event4));
      }

      TEST("should choose path 3") {
         context.getRuntimeContext().reportFailure(Result::DUPTID);
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::UNKNOWN_EVENT, action.handleEvent(context, event1));
         ASSERT_EQ(Result::UNKNOWN_EVENT, action.handleEvent(context, event2));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event4));
      }
   };
}