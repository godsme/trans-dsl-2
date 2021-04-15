//
// Created by Darwin Yuan on 2021/4/16.
//

#include <catch.hpp>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/WaitHelper.h>
#include <trans-dsl/sched/helper/AnyOfHelper.h>
#include <trans-dsl/sched/helper/SequentialHelper.h>

namespace {
    using namespace TSL_NS;

    SCENARIO("__any_of with 2 async actions") {
        __def_any_of(__asyn(AsyncAction1), __asyn(AsyncAction2)) action;

        StupidTransactionContext context{};

        const Msg1 msg1{10, 20};
        const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
        TSL_NS::Event event1{eventInfo1};

        const Msg2 msg2{30};
        const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
        TSL_NS::Event event2{eventInfo2};

        REQUIRE(Result::CONTINUE == action.exec(context));

        WHEN("event 1 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
            AND_WHEN("event 2 received, should return FATAL_BUG") {
                REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event2));
            }
        }

        WHEN("event 2 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
            AND_WHEN("event 1 received, should return FATAL_BUG") {
                REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
            }
        }

        WHEN("invoking stop, should return OUT_OF_SCOPE") {
            REQUIRE(Result::OUT_OF_SCOPE == action.stop(context, Result::OUT_OF_SCOPE));
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

    SCENARIO("__any_of with 1 async action & 1 sequential") {
        __def_any_of(__sequential(__asyn(AsyncAction1), __asyn(AsyncAction2))
                    , __asyn(AsyncAction4)) action;

        StupidTransactionContext context{};

        const Msg1 msg1{10, 20};
        const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
        TSL_NS::Event event1{eventInfo1};

        const Msg2 msg2{30};
        const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
        TSL_NS::Event event2{eventInfo2};

        const Msg4 msg4{30};
        const EV_NS::ConsecutiveEventInfo eventInfo4{EV_MSG_4, msg4};
        TSL_NS::Event event4{eventInfo4};

        REQUIRE(Result::CONTINUE == action.exec(context));

        WHEN("event 1 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            AND_WHEN("event 2 received, should return SUCCESS") {
                REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
            }
            AND_WHEN("event 4 received, should return SUCCESS") {
                REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
            }
        }

        WHEN("event 4 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
            AND_WHEN("event 1 received, should return FATAL_BUG") {
                REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
            }
        }
    }

    SCENARIO("__any_of with 1 peek action & 1 sequential") {
        __def_any_of
          ( __peek(EV_MSG_4)
          , __sequential(__asyn(AsyncAction1), __asyn(AsyncAction2))) action;

        StupidTransactionContext context{};

        const Msg1 msg1{10, 20};
        const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
        TSL_NS::Event event1{eventInfo1};

        const Msg2 msg2{30};
        const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
        TSL_NS::Event event2{eventInfo2};

        const Msg4 msg4{30};
        const EV_NS::ConsecutiveEventInfo eventInfo4{EV_MSG_4, msg4};
        TSL_NS::Event event4{eventInfo4};

        REQUIRE(Result::CONTINUE == action.exec(context));

        WHEN("event 1 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            AND_WHEN("event 2 received, should return SUCCESS") {
                REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
            }
            AND_WHEN("event 4 received, should return SUCCESS") {
                REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
            }
        }

        WHEN("event 4 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
            AND_WHEN("event 1 received, should return FATAL_BUG") {
                REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
            }
        }
    }
}