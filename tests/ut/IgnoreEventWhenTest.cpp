//
// Created by Darwin Yuan on 2021/4/26.
//

#include <catch.hpp>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/WaitHelper.h>
#include <trans-dsl/sched/helper/IgnoreEventWhenHelper.h>
#include <trans-dsl/sched/helper/SequentialHelper.h>

namespace {
    using namespace TSL_NS;

    SCENARIO("__ignore_event_when") {
        __def_ignore_event_when(EV_MSG_1, __asyn(AsyncAction2)) action;

        StupidTransactionContext context{};

        const Msg1 msg1{10, 20};
        const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
        TSL_NS::Event event1{eventInfo1};

        const Msg2 msg2{30};
        const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
        TSL_NS::Event event2{eventInfo2};

        REQUIRE(Result::CONTINUE == action.exec(context));

        WHEN("event 1 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            WHEN("event 1 received again, should return CONTINUE") {
                REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
                AND_WHEN("event 2 received, should return SUCCESS") {
                    REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
                    WHEN("event 1 received again, should return FATAL_BUG") {
                        REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
                    }
                }
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
            AND_WHEN("event received, should return FATAL_BUG") {
                REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
            }
        }

        WHEN("killed") {
            action.kill(context, Result::DUPTID);
            THEN("event 1 received, should return FATAL_BUG") {
                REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
            }
            THEN("event 2 received, should return FATAL_BUG") {
                REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event2));
            }
        }
    };

    SCENARIO("__ignore_event_when with multi-events") {
        __def_ignore_event_when(__events(EV_MSG_1, EV_MSG_3), __asyn(AsyncAction2)) action;

        StupidTransactionContext context{};

        const Msg1 msg1{10, 20};
        const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
        TSL_NS::Event event1{eventInfo1};

        const Msg2 msg2{30};
        const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
        TSL_NS::Event event2{eventInfo2};

        const Msg3 msg3{30};
        const EV_NS::ConsecutiveEventInfo eventInfo3{EV_MSG_3, msg3};
        TSL_NS::Event event3{eventInfo3};

        REQUIRE(Result::CONTINUE == action.exec(context));

        WHEN("event 1 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            WHEN("event 3 received, should return CONTINUE") {
                REQUIRE(Result::CONTINUE == action.handleEvent(context, event3));
                WHEN("event 1 received, should return CONTINUE") {
                    REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
                    AND_WHEN("event 2 received, should return SUCCESS") {
                        REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
                        WHEN("event 1 received again, should return FATAL_BUG") {
                            REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
                        }
                    }
                }
            }
        }

        WHEN("event 3 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event3));
            WHEN("event 1 received, should return CONTINUE") {
                REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
                AND_WHEN("event 2 received, should return SUCCESS") {
                    REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
                    WHEN("event 1 received again, should return FATAL_BUG") {
                        REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
                    }
                }
            }
        }
    }
}