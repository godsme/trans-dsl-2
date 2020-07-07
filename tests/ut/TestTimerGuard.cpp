//
// Created by Darwin Yuan on 2020/6/15.
//

#include <catch.hpp>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/TimeGuardHelper.h>
#include <trans-dsl/sched/helper/ProcedureHelper.h>
#include <trans-dsl/sched/helper/SyncActionHelper.h>

namespace {
   using namespace TSL_NS;

   SCENARIO("__time_guard") {
     __time_guard_t(TIMER_1, __asyn(AsyncAction1)) action;

      StupidTransactionContext context{};

      const EV_NS::SimpleEventInfo timerEvent{TIMER_EVENT_ID_1};

      REQUIRE(Result::CONTINUE == action.exec(context));
      WHEN("timeout event received, should return TIMEOUT") {
         REQUIRE(Result::TIMEOUT == action.handleEvent(context, timerEvent));
     }

      WHEN("stop should return FORCE_STOPPED") {
         REQUIRE(Result::FORCE_STOPPED == action.stop(context, Result::TIMEOUT));
     }
   };


   SCENARIO("__time_guard non-stoppable scenario") {
      using ProcedureAction =
      __procedure(
         __sync(SyncAction2),
         __finally(__asyn(AsyncAction1)));

      __time_guard_t(TIMER_1, ProcedureAction) action;

      StupidTransactionContext context{};

      const EV_NS::SimpleEventInfo timerEvent{TIMER_EVENT_ID_1};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      REQUIRE(Result::CONTINUE == action.exec(context));

      WHEN("stop, should return CONTINUE") {
         REQUIRE(Result::CONTINUE == action.stop(context, Result::OUT_OF_SCOPE));
         AND_WHEN("event 1 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
         }
         AND_WHEN("timeout event received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, timerEvent));
            AND_THEN("event 1 received, should return TIMEOUT") {
               REQUIRE(Result::TIMEOUT == action.handleEvent(context, event1));
               AND_THEN("stop again, will return FATAL_BUG") {
                  REQUIRE(Result::FATAL_BUG == action.stop(context, Result::TIMEOUT));
               }
            }
         }
      }

      WHEN("timeout event received, should return CONTINUE") {
         REQUIRE(Result::CONTINUE == action.handleEvent(context, timerEvent));
         AND_THEN("event 1 received, should return TIMEOUT") {
            REQUIRE(Result::TIMEOUT == action.handleEvent(context, event1));
         }
      }
   };

   SCENARIO("__timeout, 2 stage") {
      using ProcedureAction =
      __procedure(
         __asyn(AsyncAction2),
         __finally(__asyn(AsyncAction1)));

      __time_guard_t(TIMER_1, ProcedureAction) action;

      StupidTransactionContext context{};

      const EV_NS::SimpleEventInfo timerEventInfo{TIMER_EVENT_ID_1};
      const EV_NS::Event timerEvent{timerEventInfo};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      REQUIRE(Result::CONTINUE == action.exec(context));
      WHEN("stop should return CONTINUE") {
         REQUIRE(Result::CONTINUE == action.stop(context, Result::TIMEOUT));
         THEN("if event 1 received, should return FORCE_STOPPED") {
            REQUIRE(Result::FORCE_STOPPED == action.handleEvent(context, event1));
            AND_THEN("stop again, will return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == action.stop(context, Result::TIMEOUT));
            }
         }
      }

      GIVEN("the procedure has been in __finally stage") {
         REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));

         WHEN("event 1 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
         }
         WHEN("stopped, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.stop(context, Result::OUT_OF_SCOPE));
            AND_WHEN("timeout event received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.handleEvent(context, timerEvent));
               AND_THEN("event1 received, should return TIMEOUT") {
                  REQUIRE(Result::TIMEOUT  == action.handleEvent(context, event1));
               }
            }
         }
         WHEN("timeout event received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, timerEvent));
            AND_WHEN("stopped, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.stop(context, Result::OUT_OF_SCOPE));
               AND_THEN("event1 received, should return TIMEOUT") {
                  REQUIRE(Result::TIMEOUT  == action.handleEvent(context, event1));
               }
            }
         }
      }

      WHEN("event 1 received, should return UNKNOWN_EVENT") {
         REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event1));
         AND_WHEN("event 2 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));
            AND_WHEN("event 1 received, should return SUCCESS") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
               AND_THEN("stop again, will return FATAL_BUG") {
                  REQUIRE(Result::FATAL_BUG == action.stop(context, Result::TIMEOUT));
               }
            }
         }
      }

      WHEN("timeout event received, handleEvent should return CONTINUE") {
         REQUIRE(Result::CONTINUE == action.handleEvent(context, timerEvent));
         AND_WHEN("event 1 received, should return TIMEOUT") {
            REQUIRE(Result::TIMEOUT == action.handleEvent(context, event1));
         }
      }

      GIVEN("a stopped action") {
         REQUIRE(Result::CONTINUE == action.stop(context, Result::OUT_OF_SCOPE));
         WHEN("timeout event received, handleEvent should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, timerEvent));
            AND_WHEN("event 1 received, should return FORCE_STOPPED") {
               REQUIRE(Result::FORCE_STOPPED == action.handleEvent(context, event1));
            }
         }
      }
   };
}