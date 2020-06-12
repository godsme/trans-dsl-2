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
      , __break_if(IsTrue, Result::OUT_OF_SCOPE)
      , __sync(AsyncAction1)
      , __continue_if(IsTrue)) action;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      TEST("exec should return OUT_OF_SCOPE") {
         ASSERT_EQ(Result::OUT_OF_SCOPE, action.exec(context));
      }
   };
}