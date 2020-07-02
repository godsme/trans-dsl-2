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

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      WHEN("start with a sync action, should return SUCCESS") {
         GenericMultiThreadScheduler<__sync(SyncAction1)> scheduler;
         REQUIRE(Result::SUCCESS == scheduler.start(context));

         AND_THEN("start it again, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == scheduler.start(context));
         }

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

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      WHEN("start with a sync action, should return FAILED") {
         GenericMultiThreadScheduler<__sync(FailedSyncAction4)> scheduler;

         REQUIRE(Result::FAILED == scheduler.start(context));

         AND_THEN("start it again, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == scheduler.start(context));
         }

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

      GenericMultiThreadScheduler<__asyn(AsyncAction1)> scheduler;

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
         REQUIRE(Result::CONTINUE == scheduler.start(context));
         AND_THEN("start it again, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == scheduler.start(context));
         }

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

   SCENARIO("MultiThreadScheduler with failed async action") {
      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg3 msg3{ 10  };
      const EV_NS::ConsecutiveEventInfo event3{EV_MSG_3, msg3};

      WHEN("start, should return CONTINUE") {
         GenericMultiThreadScheduler<__asyn(FailedAsyncAction3)> scheduler;

         REQUIRE(Result::CONTINUE == scheduler.start(context));
         AND_THEN("start it again, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == scheduler.start(context));
         }

         AND_THEN("event 3 received, should return FAILED") {
            REQUIRE(Result::FAILED == scheduler.handleEvent(context, event3));
         }

         AND_THEN("event 1 received, should return UNKNOWN_EVENT") {
            REQUIRE(Result::UNKNOWN_EVENT == scheduler.handleEvent(context, event1));
            AND_WHEN("event 3 received, should return FAILED") {
               REQUIRE(Result::FAILED == scheduler.handleEvent(context, event3));
            }
         }

         AND_THEN("stop it, should return FORCE_STOPPED") {
            REQUIRE(Result::FORCE_STOPPED == scheduler.stop(context, Result::DUPTID));
         }

         AND_GIVEN("a killed scheduler") {
            scheduler.kill(context, Result::DUPTID);
            AND_THEN("start it again, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == scheduler.start(context));
            }
            THEN("event 3 received, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == scheduler.handleEvent(context, event3));
            }
            THEN("stop it, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == scheduler.stop(context, Result::DUPTID));
            }
         }
      }
   }

   SCENARIO("MultiThreadScheduler with fork action") {
      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{ 10 };
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      WHEN("start with a fork action") {
         using MainAction = __sequential(__fork(1, __asyn(AsyncAction1)), __asyn(AsyncAction2));

         GenericMultiThreadScheduler<MainAction> scheduler;
         REQUIRE(Result::CONTINUE == scheduler.start(context));

         THEN("event 1 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == scheduler.handleEvent(context, event1));
         }
      }
   }
}
