//
// Created by Darwin Yuan on 2020/7/5.
//

#include <catch.hpp>
#include <trans-dsl/trans-dsl.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <iostream>

namespace {

   using namespace TSL_NS;

   SCENARIO("__transaction with a simple sync action") {
      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      WHEN("exec with a sync action, should return SUCCESS") {
         __transaction(__sync(SyncAction1)) action;
         REQUIRE(Result::SUCCESS == action.start());
      }
   }

   SCENARIO("__transaction with multi-thread action") {
      using Trans =
         __transaction
         ( __fork(1, __asyn(AsyncAction1))
         , __fork(2, __asyn(AsyncAction4))
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


   SCENARIO("multi-thread __transaction with procedure structure") {
      using Trans =
      __transaction
      ( __fork(1, __asyn(AsyncAction1))
      , __asyn(AsyncAction2)
      , __join()
      , __finally(__asyn(AsyncAction4))
      );

      Trans trans;

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{10};
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      const Msg4 msg4{10};
      const EV_NS::ConsecutiveEventInfo event4{EV_MSG_4, msg4};

      WHEN("exec with a sync action, should return CONTINUE") {
         REQUIRE(Result::CONTINUE == trans.start());
         WHEN("event 1 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == trans.handleEvent(event1));
            THEN("event 2 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == trans.handleEvent(event2));
               THEN("event 4 received, should return SUCCESS") {
                  REQUIRE(Result::SUCCESS == trans.handleEvent(event4));
               }
            }
         }

         WHEN("event 2 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == trans.handleEvent(event2));
            THEN("event 4 received, should return UNKNOWN_EVENT") {
               REQUIRE(Result::UNKNOWN_EVENT == trans.handleEvent(event4));
               THEN("event 1 received, should return CONTINUE") {
                  REQUIRE(Result::CONTINUE == trans.handleEvent(event1));
                  THEN("event 4 received, should return SUCCESS") {
                     REQUIRE(Result::SUCCESS == trans.handleEvent(event4));
                  }
               }
            }
         }
      }
   }
}