//
// Created by Darwin Yuan on 2020/6/13.
//

#include <catch.hpp>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/OptionalHelper.h>
#include <trans-dsl/sched/helper/SyncActionHelper.h>
#include <trans-dsl/sched/helper/ProcedureHelper.h>
#include <trans-dsl/sched/helper/ConcurrentHelper.h>

namespace {
   using namespace TSL_NS;

   SCENARIO("__concurrent with 2 async actions") {
      __def_concurrent(__asyn(AsyncAction1), __asyn(AsyncAction2)) action;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      REQUIRE(Result::CONTINUE == action.exec(context));

      WHEN("event 1 received, should return CONTINUE") {
         REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
         AND_WHEN("event 2 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
         }
      }

      WHEN("event 2 received, should return CONTINUE") {
         REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));
         AND_WHEN("event 1 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
         }
      }

      WHEN("invoking stop, should return FORCE_STOPPED") {
         REQUIRE(Result::FORCE_STOPPED == action.stop(context, Result::OUT_OF_SCOPE));
         AND_WHEN("exec again, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == action.exec(context));
         }
         AND_WHEN("event received, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
         }
      }

      WHEN("killed") {
         action.kill(context, Result::DUPTID);
         THEN("start again, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == action.exec(context));
         }
         THEN("event 1 received, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
         }
         THEN("event 2 received, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event2));
         }
      }
   };

   SCENARIO("__concurrent with 2 sync actions") {
      __def_concurrent(__sync(SyncAction1), __sync(SyncAction1)) action;

      StupidTransactionContext context{};

      WHEN("exec, should return SUCCESS") {
         REQUIRE(Result::SUCCESS == action.exec(context));
      }
   };

   SCENARIO("__concurrent with a failed action") {
      __def_concurrent(__asyn(AsyncAction1), __sync(FailedSyncAction4)) action;

      StupidTransactionContext context{};

      WHEN("exec, should return FAILED") {
         REQUIRE(Result::FAILED == action.exec(context));
      }
   };

   using ProcedureAction1 =
   __procedure(__asyn(AsyncAction4), __finally(__asyn(AsyncAction2)));

   SCENARIO("__concurrent with a failed action & a procedure") {
      __def_concurrent(ProcedureAction1, __sync(FailedSyncAction4)) action;

      StupidTransactionContext context{};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      GIVEN("a clean runtime environment") {
         REQUIRE(Result::SUCCESS == context.getRuntimeEnvStatus());
         WHEN("exec, should return CONTINUE, because precedure is stopping") {
            REQUIRE(Result::CONTINUE == action.exec(context));
            THEN("should report occured error to environment") {
               REQUIRE(Result::FAILED == context.getRuntimeEnvStatus());
            }
            THEN("if event 2 received, should return FAILED") {
               REQUIRE(Result::FAILED == action.handleEvent(context, event2));
            }
            THEN("if stop, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.stop(context, Result::OUT_OF_SCOPE));
            }
         }
      }
   };

   using ProcedureAction =
   __procedure(__asyn(AsyncAction4), __finally(__on_status(Result::FORCE_STOPPED, __asyn(AsyncAction2))));

   SCENARIO("__concurrent with a immediate-stopped procedure ") {
      __def_concurrent(__sync(FailedSyncAction4), ProcedureAction) action;

      StupidTransactionContext context{};

      WHEN("exec, should return FAILED") {
         REQUIRE(Result::FAILED == action.exec(context));
      }
   };

   SCENARIO("__concurrent with a failed async action") {
      __def_concurrent(__asyn(FailedAsyncAction3), ProcedureAction) action;

      StupidTransactionContext context{};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      const Msg3 msg3{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo3{EV_MSG_3, msg3};
      TSL_NS::Event event3{eventInfo3};

      GIVEN("a clean runtime env") {
         REQUIRE(Result::SUCCESS == context.getRuntimeEnvStatus());

         WHEN("exec, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.exec(context));
            THEN("the environment keep clean") {
               REQUIRE(Result::SUCCESS == context.getRuntimeEnvStatus());
            }
            THEN("if event 3 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.handleEvent(context, event3));
               AND_THEN("should report FAILED to env") {
                  REQUIRE(Result::FAILED == context.getRuntimeEnvStatus());
               }
               AND_THEN("if event 2 received, should return FAILED") {
                  REQUIRE(Result::FAILED == action.handleEvent(context, event2));
               }
            }
            THEN("if stop, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.stop(context, Result::OUT_OF_SCOPE));
               AND_WHEN("if event 2 received, should return FORCE_STOPPED") {
                  REQUIRE(Result::FORCE_STOPPED == action.handleEvent(context, event2));
               }
               AND_WHEN("if event 3 received, should return UNKNOWN_EVENT") {
                  REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event3));
               }
               AND_WHEN("stop again, should return CONTINUE") {
                  REQUIRE(Result::CONTINUE == action.stop(context, Result::OUT_OF_SCOPE));
               }
            }
         }
      }
   };

   using ProcedureAction2 =
   __procedure(
      __asyn(AsyncAction4),
      __finally(__on_status(Result::FORCE_STOPPED, __asyn(AsyncAction2))));

   SCENARIO("__concurrent without internal error") {
      __def_concurrent(__asyn(FailedAsyncAction3), ProcedureAction2) action;

      StupidTransactionContext context{};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      REQUIRE(Result::CONTINUE == action.exec(context));
      WHEN("after stop, event3 should return CONTINUE") {
         REQUIRE(Result::CONTINUE == action.stop(context, Result::OUT_OF_SCOPE));
         AND_WHEN("event 2 received, should return FORCE_STOPPED") {
            REQUIRE(Result::FORCE_STOPPED == action.handleEvent(context, event2));
         }
      }
   };

   using ProcedureAction3 =
   __procedure(
      __asyn(FailedAsyncAction3),
      __finally(__asyn(AsyncAction2)));

   SCENARIO("__concurrent ") {
      __def_concurrent(__asyn(AsyncAction1), ProcedureAction3) action;

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      const Msg3 msg3{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo3{EV_MSG_3, msg3};
      TSL_NS::Event event3{eventInfo3};

      StupidTransactionContext context{};
      REQUIRE(Result::CONTINUE == action.exec(context));

      WHEN("event 3 received, should return CONTINUE") {
         REQUIRE(Result::CONTINUE == action.handleEvent(context, event3));
         AND_WHEN("event 1 received, should return UNKNOWN_EVENT") {
            REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event1));
            AND_THEN("if event2 received, should return FAILED") {
               REQUIRE(Result::FAILED == action.handleEvent(context, event2));
            }
         }
      }
   };

   using ProcedureAction4 =
   __procedure(
      __sync(SyncAction3),
      __finally(__asyn(AsyncAction2)));

   using ProcedureAction5 =
   __procedure(
      __sync(SyncAction2),
      __finally(__asyn(AsyncAction1)));

   SCENARIO("__concurrent with") {
      __def_concurrent(ProcedureAction4, ProcedureAction5) action;

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      StupidTransactionContext context{};

      REQUIRE(Result::CONTINUE == action.exec(context));

      WHEN("stop, should return CONTINUE") {
         REQUIRE(Result::CONTINUE == action.stop(context, Result::OUT_OF_SCOPE));
         AND_WHEN("event 1 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            AND_THEN("if event 2 received, should return SUCCESS") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
            }
         }
         AND_WHEN("event 2 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));
            AND_THEN("if event 1 received, should return SUCCESS") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
            }
         }
      }

      WHEN("event 1 received, should return CONTINUE") {
         REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
         WHEN("stop, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.stop(context, Result::OUT_OF_SCOPE));
            AND_THEN("if event 2 received, should return SUCCESS") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
            }
         }
         WHEN("event 2 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
         }
      }

      WHEN("event 2 received, should return CONTINUE") {
         REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));
         WHEN("stop, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.stop(context, Result::OUT_OF_SCOPE));
            AND_THEN("if event 1 received, should return SUCCESS") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
            }
         }
         WHEN("event 1 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
         }
      }
   };
}
