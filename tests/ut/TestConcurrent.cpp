//
// Created by Darwin Yuan on 2020/6/13.
//

#include <cctest/cctest.h>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/OptionalHelper.h>
#include <trans-dsl/sched/helper/SyncActionHelper.h>
#include <iostream>
#include <trans-dsl/sched/helper/ProcedureHelper.h>
#include <trans-dsl/sched/helper/SequentialHelper.h>
#include <trans-dsl/sched/helper/ConcurrentHelper.h>
#include <array>

namespace {
   using namespace TSL_NS;


   FIXTURE(TestConcurrent) {
      __concurrent(__async(AsyncAction1), __async(AsyncAction2)) action;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      TEST("event1 -> event2 should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event2));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event1));
      }

      TEST("event2 -> event1 should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event1));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event2));
      }
   };

   FIXTURE(TestConcurrent1) {
      __concurrent(__sync(SyncAction1), __sync(SyncAction1)) action;

      StupidTransactionContext context{};

      TEST("exec should return SUCCESS") {
         ASSERT_EQ(Result::SUCCESS, action.exec(context));
      }
   };

   FIXTURE(TestConcurrent2) {
      __concurrent(__async(AsyncAction1), __sync(FailedSyncAction4)) action;

      StupidTransactionContext context{};

      TEST("exec should return FAILED") {
         ASSERT_EQ(Result::FAILED, action.exec(context));
      }
   };

   using ProcedureAction =
   __procedure(__async(AsyncAction4), __finally(__on_status(Result::FAILED, __async(AsyncAction2))));

   FIXTURE(TestConcurrent3) {
      __concurrent(ProcedureAction, __sync(FailedSyncAction4)) action;

      StupidTransactionContext context{};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      TEST("exec should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event2));
      }

      TEST("exec should report Failure") {
         ASSERT_EQ(Result::SUCCESS, context.getStatus());
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::FAILED, context.getStatus());
      }
   };
}
