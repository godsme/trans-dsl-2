//
// Created by Darwin Yuan on 2020/7/6.
//

#include <catch.hpp>
#include <trans-dsl/trans-dsl.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <iostream>

namespace {
   using namespace TSL_NS;

   SCENARIO("__procedure with a segment") {
      using MainActions =
      __sequential
      (__sync(SyncAction1)
      , __asyn(AsyncAction1)
      , __asyn(FailedAsyncAction3)
      , __asyn(AsyncAction2)
      , __sync(SyncAction2));

      using FinalActions =
      __sequential
      ( __sync(SyncAction1)
      , __asyn(AsyncAction1)
      , __sync(SyncAction2));

      __def_procedure(
         MainActions,
         __recover(FinalActions)
      ) procedure;

      GIVEN("a protected procedure") {
         StupidTransactionContext context{};

         const Msg1 msg1{ 10, 20 };
         const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
         TSL_NS::Event event1{eventInfo1};

         const Msg3 msg3{ 30 };
         const EV_NS::ConsecutiveEventInfo eventInfo3{EV_MSG_3, msg3};
         TSL_NS::Event event3{eventInfo3};

         WHEN("exec, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            WHEN("event1 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event1));
               WHEN("event3 received, should return CONTINUE") {
                  REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event3));
                  WHEN("event 1 is received again, should return SUCCESS") {
                     REQUIRE(Result::SUCCESS == procedure.handleEvent(context, event1));
                  }
                  WHEN("stop, should return CONTINUE") {
                     REQUIRE(Result::CONTINUE == procedure.stop(context, Result::OUT_OF_SCOPE));
                     WHEN("event 1 is received again, should return SUCCESS") {
                        REQUIRE(Result::SUCCESS == procedure.handleEvent(context, event1));
                     }
                  }
               }
            }

            WHEN("event1 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event1));
               WHEN("stop, should return CONTINUE") {
                  REQUIRE(Result::CONTINUE == procedure.stop(context, Result::OUT_OF_SCOPE));
                  WHEN("event 1 is received again, should return SUCCESS") {
                     REQUIRE(Result::SUCCESS == procedure.handleEvent(context, event1));
                  }
               }
            }
         }
      }
   }
}