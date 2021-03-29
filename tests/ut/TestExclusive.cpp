//
// Created by Darwin Yuan on 2021/3/22.
//

#include <catch.hpp>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/trans-dsl.h>

namespace {
    using namespace TSL_NS;

    SCENARIO("__exclusive with 1 sync action & 1 async action") {
        __def_exclusive(__asyn(AsyncAction2), __sync(SyncAction1)) action;

        StupidTransactionContext context{};

        REQUIRE(Result::SUCCESS == action.exec(context));
    }

    SCENARIO("__exclusive with 1 async action & 1 sequential") {
        __def_exclusive( __sequential(__asyn(AsyncAction1), __asyn(AsyncAction2))
                        , __asyn(AsyncAction4)) action;

        StupidTransactionContext context{};

        const Msg1 msg1{ 10, 20 };
        const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
        TSL_NS::Event event1{eventInfo1};

        const Msg2 msg2{ 30 };
        const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
        TSL_NS::Event event2{eventInfo2};

        const Msg4 msg4{ 30 };
        const EV_NS::ConsecutiveEventInfo eventInfo4{EV_MSG_4, msg4};
        TSL_NS::Event event4{eventInfo4};

        REQUIRE(Result::CONTINUE == action.exec(context));

        WHEN("received msg4, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
        }

        WHEN("received msg1, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            WHEN("received msg4, should return UNKNOWN_EVENT") {
                REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event4));
            }
            WHEN("received msg2, should return SUCCESS") {
                REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
            }
            WHEN("stop, should return OUT_OF_SCOPE") {
                REQUIRE(Result::OUT_OF_SCOPE == action.stop(context, Result::OUT_OF_SCOPE));
            }
        }
    }

    SCENARIO("__exclusive with 1 async action & 1 procedure") {
        __def_exclusive( __procedure
                            ( __asyn(AsyncAction1)
                            , __asyn(FailedAsyncAction3)
                            , __finally(__asyn(AsyncAction2)))
        , __asyn(AsyncAction4)) action;

        StupidTransactionContext context{};

        const Msg1 msg1{ 10, 20 };
        const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
        TSL_NS::Event event1{eventInfo1};

        const Msg2 msg2{ 30 };
        const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
        TSL_NS::Event event2{eventInfo2};

        const Msg2 msg3{ 30 };
        const EV_NS::ConsecutiveEventInfo eventInfo3{EV_MSG_3, msg3};
        TSL_NS::Event event3{eventInfo3};

        const Msg4 msg4{ 30 };
        const EV_NS::ConsecutiveEventInfo eventInfo4{EV_MSG_4, msg4};
        TSL_NS::Event event4{eventInfo4};

        REQUIRE(Result::CONTINUE == action.exec(context));

        WHEN("received msg4, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
        }

        WHEN("received msg1, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            WHEN("received msg4, should return UNKNOWN_EVENT") {
                REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event4));
            }
            WHEN("received msg3, should return CONTINUE") {
                REQUIRE(Result::CONTINUE == action.handleEvent(context, event3));
                WHEN("received msg2, should return FAILED") {
                    REQUIRE(Result::FAILED == action.handleEvent(context, event2));
                }
            }
            WHEN("stop, should return CONTINUE") {
                REQUIRE(Result::CONTINUE == action.stop(context, Result::OUT_OF_SCOPE));
                WHEN("received msg2, should return OUT_OF_SCOPE") {
                    REQUIRE(Result::OUT_OF_SCOPE == action.handleEvent(context, event2));
                }
            }
        }
    }

    SCENARIO("__exclusive with 2 async actions") {
        __def_exclusive(__asyn(AsyncAction1), __asyn(AsyncAction2)) action;

        StupidTransactionContext context{};

        const Msg1 msg1{ 10, 20 };
        const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
        TSL_NS::Event event1{eventInfo1};

        const Msg2 msg2{ 30 };
        const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
        TSL_NS::Event event2{eventInfo2};

        REQUIRE(Result::CONTINUE == action.exec(context));

        WHEN("event 1 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
        }

        WHEN("event 2 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
        }

        WHEN("invoking stop, should return OUT_OF_SCOPE") {
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
}