//
// Created by Darwin Yuan on 2020/6/11.
//

#include <catch.hpp>
#include <event/impl/ConsecutiveEventInfo.h>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/SequentialHelper.h>
#include <trans-dsl/sched/helper/SyncActionHelper.h>
#include <trans-dsl/sched/helper/ProcedureHelper.h>
#include <trans-dsl/sched/helper/OptionalHelper.h>
#include <trans-dsl/sched/helper/WaitHelper.h>
#include <iostream>

namespace {
   using namespace TSL_NS;

   SCENARIO("TestProcedure") {
        using MainActions =
         __sequential
           (__sync(SyncAction1)
           , __asyn(AsyncAction1)
           , __asyn(FailedAsyncAction3)
           , __asyn(AsyncAction2)
           , __sync(SyncAction2));

        using FinalActions =
         __sequential
           (__sync(SyncAction1)
           , __asyn(AsyncAction1)
           , __sync(FailedSyncAction4)
           , __asyn(AsyncAction2)
           , __sync(SyncAction2));

        __def_procedure(
         MainActions,
         __finally(FinalActions)
        ) procedure;

        StupidTransactionContext context{};

        const Msg1 msg1{ 10, 20 };
        const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
        TSL_NS::Event event1{eventInfo1};

        const Msg3 msg3{ 30 };
        const EV_NS::ConsecutiveEventInfo eventInfo3{EV_MSG_3, msg3};
        TSL_NS::Event event3{eventInfo3};

        WHEN("exec should return CONTINUE") {
            std::cout << sizeof(procedure) <<  " " << sizeof(FinalActions)  << std::endl;
            REQUIRE(Result::CONTINUE == procedure.exec(context));
        }

        WHEN("call exec again, should return FATAL_BUG") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::FATAL_BUG == procedure.exec(context));
        }

        WHEN("call exec -> handleEvent(event1), should return CONTINUE") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event1));
        }

        WHEN("call exec -> event1 -> event3, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event1));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event3));
        }

        WHEN("call exec -> event1 -> stop, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event1));
            REQUIRE(Result::CONTINUE == procedure.stop(context, Result::OUT_OF_SCOPE));
        }

        WHEN("call exec -> event1 -> stop -> stop, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event1));
            REQUIRE(Result::CONTINUE == procedure.stop(context, Result::OUT_OF_SCOPE));
            REQUIRE(Result::CONTINUE == procedure.stop(context, Result::OUT_OF_SCOPE));
        }

        WHEN("call exec -> event1 -> kill -> event3, should return FATAL_BUG") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event1));
            procedure.kill(context, Result::DUPTID);
            REQUIRE(Result::FATAL_BUG == procedure.handleEvent(context, event3));
        }

        WHEN("call exec -> event1 -> kill -> stop, should return FATAL_BUG") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event1));
            procedure.kill(context, Result::DUPTID);
            REQUIRE(Result::FATAL_BUG == procedure.stop(context, Result::OUT_OF_SCOPE));
        }

        WHEN("call exec -> event1 -> event3 -> event1, should return FAILED") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event1));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event3));
            REQUIRE(Result::FAILED   == procedure.handleEvent(context, event1));
        }

        WHEN("stop a failed procedure should return FATAL_BUG") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event1));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event3));
            REQUIRE(Result::FAILED   == procedure.handleEvent(context, event1));
            REQUIRE(Result::FATAL_BUG == procedure.stop(context, Result::OUT_OF_SCOPE));
        }
   };

    SCENARIO("TestProcedure1") {
      __def_procedure(__sequential
        (__sync(SyncAction1)
        , __asyn(FailedAsyncAction3)
        , __asyn(AsyncAction2)
        , __sync(SyncAction2)),
        __finally(__sequential
          (__sync(SyncAction1)
          , __asyn(AsyncAction1)
          , __sync(SyncAction2)))
      ) procedure;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg3 msg3{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo3{EV_MSG_3, msg3};
      TSL_NS::Event event3{eventInfo3};

        WHEN("exec should return CONTINUE") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
        }

        WHEN("call exec again, should return FATAL_BUG") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::FATAL_BUG == procedure.exec(context));
        }

        WHEN("call exec -> event1, should return UNKNOWN_EVENT") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::UNKNOWN_EVENT == procedure.handleEvent(context, event1));
        }

        WHEN("call exec -> event3, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event3));
        }

        WHEN("call exec -> event3 -> event1, should return FAILED") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event3));
            REQUIRE(Result::FAILED   == procedure.handleEvent(context, event1));
        }
   };

   using MainProcedure =
   __procedure(__sequential
     (__sync(SyncAction1)
     , __asyn(AsyncAction1)
     , __sync(SyncAction2)),
     __finally(__sequential
        (__sync(SyncAction1)
        , __asyn(AsyncAction2)
        , __sync(SyncAction2))));

    SCENARIO("TestProcedure3") {
      __def_procedure(MainProcedure,
      __finally(__sequential
        (__sync(SyncAction1)
        , __asyn(AsyncAction1)
        , __sync(SyncAction2)))
      ) procedure;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      const Msg3 msg3{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo3{EV_MSG_3, msg3};
      TSL_NS::Event event3{eventInfo3};

        WHEN("exec should return CONTINUE") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
        }

        WHEN("call exec again, should return FATAL_BUG") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::FATAL_BUG == procedure.exec(context));
        }

        WHEN("call exec -> stop, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.stop(context, Result::OUT_OF_SCOPE));
        }

        WHEN("call exec -> stop -> stop, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.stop(context, Result::OUT_OF_SCOPE));
            REQUIRE(Result::CONTINUE == procedure.stop(context, Result::OUT_OF_SCOPE));
            REQUIRE(Result::CONTINUE == procedure.stop(context, Result::OUT_OF_SCOPE));
        }

        WHEN("call exec -> stop -> event2, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.stop(context, Result::OUT_OF_SCOPE));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event2));
        }

        WHEN("call exec -> stop -> event2 -> event1, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.stop(context, Result::OUT_OF_SCOPE));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event2));
            REQUIRE(Result::OUT_OF_SCOPE == procedure.handleEvent(context, event1));
        }

        WHEN("after success, if handleEvent, should return FATAL_BUG") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.stop(context, Result::OUT_OF_SCOPE));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event2));
            REQUIRE(Result::OUT_OF_SCOPE == procedure.handleEvent(context, event1));
            REQUIRE(Result::FATAL_BUG == procedure.handleEvent(context, event1));
        }

        WHEN("after success, if stop, should return FATAL_BUG") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.stop(context, Result::OUT_OF_SCOPE));
            REQUIRE(Result::CONTINUE == procedure.handleEvent(context, event2));
            REQUIRE(Result::OUT_OF_SCOPE == procedure.handleEvent(context, event1));
            REQUIRE(Result::FATAL_BUG == procedure.stop(context, Result::OUT_OF_SCOPE));
        }
   };

    SCENARIO("TestProcedure4") {
      __def_procedure(
         __sync(FailedSyncAction4),
         __finally(__on_fail(__asyn(AsyncAction1)))
      ) procedure;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

        WHEN("after exec, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::FAILED   == context.getRuntimeEnvStatus());
        }

        WHEN("after exec -> event1, should return SUCCESS") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::FAILED   == procedure.handleEvent(context, event1));
            REQUIRE(Result::FAILED   == context.getRuntimeEnvStatus());
        }
   };

    SCENARIO("TestProcedure5") {
      __def_procedure(
         __asyn(AsyncAction2),
         __finally(__on_status(Result::OUT_OF_SCOPE, __asyn(AsyncAction1)))
      ) procedure;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

        WHEN("after exec -> event2, should return SUCCESS") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::SUCCESS  == procedure.handleEvent(context, event2));
        }

        WHEN("after exec, if stop, should return OUT_OF_SCOPE") {
            REQUIRE(Result::CONTINUE == procedure.exec(context));
            REQUIRE(Result::CONTINUE == procedure.stop(context, Result::OUT_OF_SCOPE));
            REQUIRE(Result::OUT_OF_SCOPE == procedure.handleEvent(context, event1));
      }

        WHEN("abc") {
//         __procedure
//            ( __sequential
//               ( __wait(1)
//               , __wait(2)
//               , __wait(3)
//               , __wait(4)
//               , __wait(5)
//               , __wait(6)
//               , __wait(7)
//               )
//            , __finally( __sequential(__wait(3), __wait(4), __wait(5), __wait(6)))
//            ) a;
//
//         std::cout << sizeof(a) << std::endl;
      }
   };


}