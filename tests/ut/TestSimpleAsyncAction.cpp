//
// Created by Darwin Yuan on 2020/6/10.
//

#include <catch.hpp>
#include <trans-dsl/trans-dsl.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"

namespace {
   using namespace TSL_NS;

   const Msg1 msg1{10, 20};
   const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

   SCENARIO("SimpleAsyncAction") {
      AsyncAction1 action;
      StupidTransactionContext context{};

      GIVEN("exec has not been invoked") {
         WHEN("event1 is received, should return USER_FATAL_BUG") {
            REQUIRE(Result::USER_FATAL_BUG == action.handleEvent(context, event1));
         }
      }

      GIVEN("exec has been invoked") {
         REQUIRE(Result::CONTINUE == action.exec(context));
         WHEN("exec is called again, it should return OUT_OF_SCOPE") {
            REQUIRE(Result::OUT_OF_SCOPE == action.exec(context));
         }

         WHEN("event1 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));

            AND_WHEN("event1 received again, should return USER_FATAL_BUG") {
               REQUIRE(Result::USER_FATAL_BUG == action.handleEvent(context, event1));
            }
         }

         WHEN("an unexpected event receivedm should return UNKNOWN_EVENT") {
            REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, EV_NS::SimpleEventInfo(100)));
         }

         WHEN("the event is accepted") {
            Event event{event1};
            REQUIRE_FALSE(event.isConsumed());
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event));

            THEN("the event should be consumed") {
               REQUIRE(true == event.isConsumed());
            }

            THEN("if trying to kill it, should return USER_FATAL_BUG") {
               REQUIRE(Result::USER_FATAL_BUG == action.kill(context, Result::TIMEOUT));
            }
         }

         WHEN("the action is killed") {
            REQUIRE(Result::SUCCESS == action.kill(context, Result::DUPTID));

            THEN("if event1 is received, should return USER_FATAL_BUG") {
               REQUIRE(Result::USER_FATAL_BUG == action.handleEvent(context, event1));
            }
         }
      }
   }

   SCENARIO("__asyn(AsyncAction)") {
      __asyn(AsyncAction1) action;
      StupidTransactionContext context{};

      GIVEN("exec has not been invoked") {
         WHEN("event1 is received, should return USER_FATAL_BUG") {
            REQUIRE(Result::USER_FATAL_BUG == action.handleEvent(context, event1));
         }

         WHEN("stop it, should return USER_FATAL_BUG") {
            REQUIRE(Result::USER_FATAL_BUG == action.stop(context, Result::DUPTID));
         }
      }
   }
}
