//
// Created by Darwin Yuan on 2020/6/11.
//

#include <catch.hpp>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/OptionalHelper.h>

namespace {
   using namespace TSL_NS;

   bool is_true(const TransactionInfo&) { return true; }

    SCENARIO("TestOptional") {
        __def_optional(is_true, __asyn(AsyncAction1)) optional;

        StupidTransactionContext context{};

        const Msg1 msg1{10, 20};
        const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
        TSL_NS::Event event1{eventInfo1};

        WHEN("exec should return CONTINUE") {
         REQUIRE(Result::CONTINUE == optional.exec(context));
        }

        WHEN("exec -> event1 should return SUCCESS") {
            REQUIRE(Result::CONTINUE == optional.exec(context));
            REQUIRE(Result::SUCCESS == optional.handleEvent(context, event1));
        }
   };

   auto is_false = [](const TransactionInfo&) { return false; };

    SCENARIO("TestOptional1") {
        __def_optional(is_false, __asyn(AsyncAction1)) optional;

        const Msg1 msg1{10, 20};
        const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
        TSL_NS::Event event1{eventInfo1};

        WHEN("exec should return SUCCESS") {
            StupidTransactionContext context{};
            REQUIRE(Result::SUCCESS == optional.exec(context));
        }

        WHEN("exec -> event1 should return FATAL_BUG") {
            StupidTransactionContext context{};
            REQUIRE(Result::SUCCESS == optional.exec(context));
            REQUIRE(Result::FATAL_BUG == optional.handleEvent(context, event1));
         }
   };

   struct PredTrue {
      bool operator()(const TransactionInfo&) { return true; }
   };

    SCENARIO("TestOptional2") {
      __def_optional(PredTrue, __asyn(AsyncAction1)) optional;

      StupidTransactionContext context{};

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

        WHEN("exec should return CONTINUE") {
            REQUIRE(Result::CONTINUE == optional.exec(context));
        }

        WHEN("exec -> event1 should return SUCCESS") {
            REQUIRE(Result::CONTINUE == optional.exec(context));
            REQUIRE(Result::SUCCESS  == optional.handleEvent(context, event1));
        }
   };

   struct PredFalse {
      bool operator()(const TransactionInfo&) { return false; }
   };

    SCENARIO("TestOptional3") {
      __def_optional(PredFalse, __asyn(AsyncAction1)) optional;

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

        WHEN("exec should return SUCCESS") {
            StupidTransactionContext context{};
            REQUIRE(Result::SUCCESS == optional.exec(context));
        }

        WHEN("exec -> event1 should return FATAL_BUG") {
            StupidTransactionContext context{};
            REQUIRE(Result::SUCCESS == optional.exec(context));
            REQUIRE(Result::FATAL_BUG == optional.handleEvent(context, event1));
      }
   };

    SCENARIO("TestOptional4") {
        __def_optional(__is_status(Result::OUT_OF_SCOPE), __asyn(AsyncAction1)) optional;

        const Msg1 msg1{10, 20};
        const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
        TSL_NS::Event event1{eventInfo1};

        StupidTransactionContext context{};
        WHEN("exec should return SUCCESS") {
            REQUIRE(Result::SUCCESS == optional.exec(context));
        }

        WHEN("exec should return CONTINUE") {
            context.reportFailure(Result::OUT_OF_SCOPE);
            REQUIRE(Result::CONTINUE == optional.exec(context));
            REQUIRE(Result::SUCCESS  == optional.handleEvent(context, event1));
        }
   };
}