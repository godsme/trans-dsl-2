//
// Created by Darwin Yuan on 2020/6/12.
//

#include <cctest/cctest.h>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/OptionalHelper.h>
#include <trans-dsl/sched/helper/LoopHelper.h>
#include <trans-dsl/sched/helper/SyncActionHelper.h>
#include <iostream>

namespace {
   using namespace TSL_NS;

   bool is_true(const TransactionInfo&) { return true; }
   bool is_false(const TransactionInfo&) { return false; }

   struct IsTrue {
      bool operator()(const TransactionInfo&) {
         return true;
      }

      int i = 0;
   };

   FIXTURE(TestLoop) {
     __loop
        ( __async(AsyncAction1)
        , __break_if(IsTrue, Result::OUT_OF_SCOPE)
        , __sync(SyncAction3)
        , __continue_if(IsTrue)) action;

     StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

     TEST("exec should return CONTINUE") {
        ASSERT_EQ(Result::CONTINUE, action.exec(context));
     }

      TEST("exec -> handleEvent should return OUT_OF_SCOPE") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::OUT_OF_SCOPE, action.handleEvent(context, event1));
      }
   };

   FIXTURE(TestLoop1) {
      __loop
      ( __sync(SyncAction1)
      , __break_if(is_true, Result::OUT_OF_SCOPE)
      , __async(AsyncAction1)
      , __continue_if(IsTrue)) action;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      TEST("exec should return OUT_OF_SCOPE") {
         ASSERT_EQ(Result::OUT_OF_SCOPE, action.exec(context));
      }
   };

   FIXTURE(TestLoop2) {
      __loop
      ( __async(AsyncAction1)
      , __break_if(is_false, Result::OUT_OF_SCOPE)
      , __async(AsyncAction2)
      , __continue_if(IsTrue)) action;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      TEST("should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event1));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event2));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event1));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event2));
      }
   };

   FIXTURE(TestLoop3) {
      __loop
      ( __async(FailedAsyncAction3)
      , __async(AsyncAction1)
      , __break_if(__is_failed, Result::OUT_OF_SCOPE)
      , __async(AsyncAction2)
      , __continue_if(IsTrue)) action;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      const Msg3 msg3{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo3{EV_MSG_3, msg3};
      TSL_NS::Event event3{eventInfo3};

      TEST("should return OUT_OF_SCOPE") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::OUT_OF_SCOPE, action.handleEvent(context, event3));
      }
   };
}