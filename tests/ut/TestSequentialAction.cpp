//
// Created by Darwin Yuan on 2020/6/10.
//

#include <catch.hpp>
#include <event/impl/ConsecutiveEventInfo.h>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/SequentialHelper.h>
#include <trans-dsl/sched/helper/SafeHelper.h>
#include <trans-dsl/sched/helper/SyncActionHelper.h>
#include <iostream>

namespace {
   using namespace TSL_NS;

   SCENARIO("SequentialAction") {
        using Action =
        __def_sequential(
         __sync(SyncAction1),
         __sequential(__asyn(AsyncAction1),
         __sync(SyncAction3),
         __asyn(AsyncAction2)),
         __sync(SyncAction2)
         );

        Action action;
        StupidTransactionContext context{};

//      S<typename TSL_NS::details::Sequential<__sync(SyncAction1),
//         __sequential(__asyn(AsyncAction1),
//                      __sync(SyncAction3),
//                      __asyn(AsyncAction2)),
//         __sync(SyncAction2)>::Base> s;
//

        const Msg1 msg1{ 10, 20 };
        const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
        TSL_NS::Event event1{eventInfo1};

        const Msg2 msg2{ 30 };
        const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
        TSL_NS::Event event2{eventInfo2};

        WHEN("exec should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.exec(context));
        }

        WHEN("handleEvent(event1) should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.exec(context));
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
        }

        WHEN("handleEvent(event2) should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.exec(context));
            REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event2));
        }

        WHEN("handleEvent(event1) -> handleEvent(event2) should return SUCCESS") {
            REQUIRE(Result::CONTINUE == action.exec(context));
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
        }

        WHEN("handleEvent(event1) -> stop should return FORCE_STOPPED") {
            REQUIRE(Result::CONTINUE == action.exec(context));
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            REQUIRE(Result::OUT_OF_SCOPE == action.stop(context, Result::OUT_OF_SCOPE));
        }

        WHEN("after stop, handleEvent should return FATAL_BUG") {
            REQUIRE(Result::CONTINUE == action.exec(context));
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            REQUIRE(Result::OUT_OF_SCOPE == action.stop(context, Result::OUT_OF_SCOPE));
            REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event2));
        }

        WHEN("after success, handleEvent should return FATAL_BUG") {
            REQUIRE(Result::CONTINUE == action.exec(context));
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
            REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event2));
        }

        WHEN("after kill, handleEvent should return FATAL_BUG") {
            REQUIRE(Result::CONTINUE == action.exec(context));
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            action.kill(context, Result::FAILED);
            REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event2));
        }

        WHEN("after kill, handleEvent should return FATAL_BUG") {
            REQUIRE(Result::CONTINUE == action.exec(context));
            action.kill(context, Result::DUPTID);
            REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
        }

        WHEN("after stop, call exec will return FATAL_BUG") {
            REQUIRE(Result::CONTINUE == action.exec(context));
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            REQUIRE(Result::OUT_OF_SCOPE == action.stop(context, Result::OUT_OF_SCOPE));
            REQUIRE(Result::FATAL_BUG == action.exec(context));
        }

        WHEN("after kill, call exec will return FATAL_BUG") {
            REQUIRE(Result::CONTINUE == action.exec(context));
            action.kill(context, Result::DUPTID);
            REQUIRE(Result::FATAL_BUG == action.exec(context));
        }
   };

   SCENARIO("Last Safe Sequential") {
        using Action =
        __def_sequential(
                __sync(SyncAction1),
                __safe(__asyn(AsyncAction1))
        );

        Action action;
        StupidTransactionContext context{};

        const Msg1 msg1{ 10, 20 };
        const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
        TSL_NS::Event event1{eventInfo1};

        WHEN("exec should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.exec(context));

            WHEN("Stop, should return CONTINUE") {
                REQUIRE(Result::CONTINUE == action.stop(context, Result::DUPTID));

                WHEN("recved Msg1, should return SUCCESS") {
                    REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
                }
            }
        }
   }

    SCENARIO("First Safe Sequential") {
        using Action =
        __def_sequential(
                __safe(__asyn(AsyncAction1)),
                __sync(SyncAction1),
                __sync(SyncAction2)
        );

        Action action;
        StupidTransactionContext context{};

        const Msg1 msg1{ 10, 20 };
        const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
        TSL_NS::Event event1{eventInfo1};

        WHEN("exec should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.exec(context));

            WHEN("Stop, should return CONTINUE") {
                REQUIRE(Result::CONTINUE == action.stop(context, Result::DUPTID));

                WHEN("recved Msg1, should return DUPTID") {
                    REQUIRE(Result::DUPTID == action.handleEvent(context, event1));
                }
            }
        }
    }
}