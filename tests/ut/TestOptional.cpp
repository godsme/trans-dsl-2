//
// Created by Darwin Yuan on 2020/6/11.
//

#include <cctest/cctest.h>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/OptionalHelper.h>

namespace {
   using namespace TSL_NS;

   bool is_true(const TransactionInfo&) { return true; }

   FIXTURE(TestOptional) {
      __optional(is_true, __asyn(AsyncAction1)) optional;

      StupidTransactionContext context{};

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      TEST("exec should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, optional.exec(context));
      }

      TEST("exec -> event1 should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, optional.exec(context));
         ASSERT_EQ(Result::SUCCESS, optional.handleEvent(context, event1));
      }
   };

   auto is_false = [](const TransactionInfo&) { return false; };

   FIXTURE(TestOptional1) {
      __optional(is_false, __asyn(AsyncAction1)) optional;

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      TEST("exec should return SUCCESS") {
         StupidTransactionContext context{};
         ASSERT_EQ(Result::SUCCESS, optional.exec(context));
      }

      TEST("exec -> event1 should return FATAL_BUG") {
         StupidTransactionContext context{};
         ASSERT_EQ(Result::SUCCESS, optional.exec(context));
         ASSERT_EQ(Result::FATAL_BUG, optional.handleEvent(context, event1));
      }
   };

   struct PredTrue {
      bool operator()(const TransactionInfo&) { return true; }
   };

   FIXTURE(TestOptional2) {
      __optional(PredTrue, __asyn(AsyncAction1)) optional;

      StupidTransactionContext context{};

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      TEST("exec should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, optional.exec(context));
      }

      TEST("exec -> event1 should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, optional.exec(context));
         ASSERT_EQ(Result::SUCCESS, optional.handleEvent(context, event1));
      }
   };

   struct PredFalse {
      bool operator()(const TransactionInfo&) { return false; }
   };

   FIXTURE(TestOptional3) {
      __optional(PredFalse, __asyn(AsyncAction1)) optional;

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      TEST("exec should return SUCCESS") {
         StupidTransactionContext context{};
         ASSERT_EQ(Result::SUCCESS, optional.exec(context));
      }

      TEST("exec -> event1 should return FATAL_BUG") {
         StupidTransactionContext context{};
         ASSERT_EQ(Result::SUCCESS, optional.exec(context));
         ASSERT_EQ(Result::FATAL_BUG, optional.handleEvent(context, event1));
      }
   };

   FIXTURE(TestOptional4) {
      __optional(__is_status(Result::OUT_OF_SCOPE), __asyn(AsyncAction1)) optional;

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      StupidTransactionContext context{};
      TEST("exec should return SUCCESS") {
         ASSERT_EQ(Result::SUCCESS, optional.exec(context));
      }

      TEST("exec should return CONTINUE") {
         context.reportFailure(Result::OUT_OF_SCOPE);
         ASSERT_EQ(Result::CONTINUE, optional.exec(context));
         ASSERT_EQ(Result::SUCCESS, optional.handleEvent(context, event1));
      }
   };
}