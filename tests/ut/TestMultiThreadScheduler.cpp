//
// Created by Darwin Yuan on 2020/7/2.
//

#include <catch.hpp>
#include <trans-dsl/sched/action/MultiThreadScheduler.h>
#include <trans-dsl/trans-dsl.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"

namespace {

   using namespace TSL_NS;

   SCENARIO("MultiThreadScheduler with successful sync action") {
      StupidTransactionContext context{};

      MultiThreadScheduler scheduler;

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      WHEN("start with a sync action, should return SUCCESS") {
         __sync(SyncAction1) action;
         REQUIRE(Result::SUCCESS == scheduler.start(context, action));

         AND_WHEN("event 1 received, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == scheduler.handleEvent(context, event1));
         }
         AND_WHEN("stop it, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == scheduler.stop(context, Result::DUPTID));
         }
      }
   }

   SCENARIO("MultiThreadScheduler with failed sync action") {
      StupidTransactionContext context{};

      MultiThreadScheduler scheduler;

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      WHEN("start with a sync action, should return FAILED") {
         __sync(FailedSyncAction4) action;
         REQUIRE(Result::FAILED == scheduler.start(context, action));

         AND_WHEN("event 1 received, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == scheduler.handleEvent(context, event1));
         }
         AND_WHEN("stop it, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == scheduler.stop(context, Result::DUPTID));
         }
      }
   }

   SCENARIO("MultiThreadScheduler with successful async action") {
      StupidTransactionContext context{};

      MultiThreadScheduler scheduler;

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{ 10  };
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      WHEN("event 1 received, should return FATAL_BUG") {
         REQUIRE(Result::FATAL_BUG == scheduler.handleEvent(context, event1));
      }

      WHEN("stop it, should return FATAL_BUG") {
         REQUIRE(Result::FATAL_BUG == scheduler.stop(context, Result::TIMEOUT));
      }

      WHEN("start with a sync action, should return CONTINUE") {
         __asyn(AsyncAction1) action;
         REQUIRE(Result::CONTINUE == scheduler.start(context, action));

         AND_THEN("event 1 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == scheduler.handleEvent(context, event1));
         }
         AND_THEN("stop it, should return FORCE_STOPPED") {
            REQUIRE(Result::FORCE_STOPPED == scheduler.stop(context, Result::DUPTID));
         }
         AND_THEN("event 2 received, should return UNKNOWN_EVENT") {
            REQUIRE(Result::UNKNOWN_EVENT == scheduler.handleEvent(context, event2));
            AND_WHEN("event 1 received, should return SUCCESS") {
               REQUIRE(Result::SUCCESS == scheduler.handleEvent(context, event1));
            }
         }
         AND_GIVEN("a killed scheduler") {
            scheduler.kill(context, Result::DUPTID);
            THEN("stop it, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == scheduler.stop(context, Result::DUPTID));
            }
         }
      }
   }
}
