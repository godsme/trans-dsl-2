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

   __def(Fork2, __params(__action(ACTION1), __action(ACTION2))) __as
   ( __fork(1, __asyn(ACTION1))
   , __fork(2, __asyn(ACTION2)));

   template<typename T> struct S;

   SCENARIO("__transaction with a segment") {

      using Trans =
      __transaction
      ( __apply(Fork2, __with(AsyncAction1, AsyncAction4))
      , __asyn(AsyncAction2)
      , __join()
      );

      Trans trans;

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{10};
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      const Msg4 msg4{10};
      const EV_NS::ConsecutiveEventInfo event4{EV_MSG_4, msg4};

      WHEN("start, should return CONTINUE") {
         REQUIRE(Result::CONTINUE == trans.start());
         WHEN("event 2 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == trans.handleEvent(event2));
            THEN("event 4 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == trans.handleEvent(event4));
               THEN("event 1 received, should return SUCCESS") {
                  REQUIRE(Result::SUCCESS == trans.handleEvent(event1));
               }
            }
         }
      }

      WHEN("exec with a sync action, should return CONTINUE") {
         REQUIRE(Result::CONTINUE == trans.start());
         WHEN("event 1 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == trans.handleEvent(event1));
            THEN("event 4 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == trans.handleEvent(event4));
               THEN("event 2 received, should return SUCCESS") {
                  REQUIRE(Result::SUCCESS == trans.handleEvent(event2));
               }
            }
         }
      }
   }
}