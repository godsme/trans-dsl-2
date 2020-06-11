//
// Created by Darwin Yuan on 2020/6/11.
//

#include <cctest/cctest.h>
#include <event/impl/ConsecutiveEventInfo.h>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/SequentialHelper.h>
#include <trans-dsl/sched/helper/SyncActionHelper.h>
#include <trans-dsl/sched/helper/ProcedureHelper.h>
#include <trans-dsl/sched/helper/OptionalHelper.h>

namespace {
   using namespace TSL_NS;



   FIXTURE(TestProcedure) {
      __procedure(__sequential
        ( __call(SyncAction1)
        , __async(AsyncAction1)
        , __async(FailedAsyncAction3)
        , __async(AsyncAction2)
        , __call(SyncAction2)),
      __finally(__sequential
        ( __call(SyncAction1)
        , __async(AsyncAction1)
        , __call(FailedSyncAction4)
        , __async(AsyncAction2)
        , __call(SyncAction2)))
      ) procedure;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg3 msg3{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo3{EV_MSG_3, msg3};
      TSL_NS::Event event3{eventInfo3};

      TEST("exec should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
      }

      TEST("call exec again, should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::FATAL_BUG, procedure.exec(context));
      }

      TEST("call exec -> handleEvent(event1), should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event1));
      }

      TEST("call exec -> event1 -> event3, should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event1));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event3));
      }

      TEST("call exec -> event1 -> stop, should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event1));
         ASSERT_EQ(Result::CONTINUE, procedure.stop(context));
      }

      TEST("call exec -> event1 -> stop -> stop, should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event1));
         ASSERT_EQ(Result::CONTINUE, procedure.stop(context));
         ASSERT_EQ(Result::CONTINUE, procedure.stop(context));
      }

      TEST("call exec -> event1 -> kill -> event3, should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event1));
         procedure.kill(context);
         ASSERT_EQ(Result::FATAL_BUG, procedure.handleEvent(context, event3));
      }

      TEST("call exec -> event1 -> kill -> stop, should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event1));
         procedure.kill(context);
         ASSERT_EQ(Result::FATAL_BUG, procedure.stop(context));
      }

      TEST("call exec -> event1 -> event3 -> event1, should return FAILED") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event1));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event3));
         ASSERT_EQ(Result::FAILED, procedure.handleEvent(context, event1));
      }

      TEST("stop a failed procedure should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event1));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event3));
         ASSERT_EQ(Result::FAILED, procedure.handleEvent(context, event1));
         ASSERT_EQ(Result::FATAL_BUG, procedure.stop(context));
      }
   };

   FIXTURE(TestProcedure1) {
      __procedure(__sequential
        ( __call(SyncAction1)
        , __async(FailedAsyncAction3)
        , __async(AsyncAction2)
        , __call(SyncAction2)),
        __finally(__sequential
          ( __call(SyncAction1)
          , __async(AsyncAction1)
          , __call(SyncAction2)))
      ) procedure;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg3 msg3{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo3{EV_MSG_3, msg3};
      TSL_NS::Event event3{eventInfo3};

      TEST("exec should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
      }

      TEST("call exec again, should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::FATAL_BUG, procedure.exec(context));
      }

      TEST("call exec -> event1, should return UNKNOWN_EVENT") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::UNKNOWN_EVENT, procedure.handleEvent(context, event1));
      }

      TEST("call exec -> event3, should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event3));
      }

      TEST("call exec -> event3 -> event1, should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event3));
         ASSERT_EQ(Result::SUCCESS, procedure.handleEvent(context, event1));
      }
   };

   constexpr int* P = nullptr;
   FIXTURE(TestProcedure3) {
      __procedure(__procedure(__sequential
        ( __sync(SyncAction1)
        , __async(AsyncAction1)
        , __sync(SyncAction2)),
        __finally(__sequential
          ( __sync(SyncAction1)
          , __async(AsyncAction2)
          , __sync(SyncAction2)))),
      __finally(__sequential
        ( __sync(SyncAction1)
        , __async(AsyncAction1)
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

      TEST("exec should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
      }

      TEST("call exec again, should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::FATAL_BUG, procedure.exec(context));
      }

      TEST("call exec -> stop, should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.stop(context));
      }

      TEST("call exec -> stop -> stop, should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.stop(context));
         ASSERT_EQ(Result::CONTINUE, procedure.stop(context));
         ASSERT_EQ(Result::CONTINUE, procedure.stop(context));
      }

      TEST("call exec -> stop -> event2, should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.stop(context));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event2));
      }

      TEST("call exec -> stop -> event2 -> event1, should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.stop(context));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event2));
         ASSERT_EQ(Result::SUCCESS, procedure.handleEvent(context, event1));
      }

      TEST("after success, if handleEvent, should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.stop(context));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event2));
         ASSERT_EQ(Result::SUCCESS, procedure.handleEvent(context, event1));
         ASSERT_EQ(Result::FATAL_BUG, procedure.handleEvent(context, event1));
      }

      TEST("after success, if stop, should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.stop(context));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event2));
         ASSERT_EQ(Result::SUCCESS, procedure.handleEvent(context, event1));
         ASSERT_EQ(Result::FATAL_BUG, procedure.stop(context));
      }


   };

//   FIXTURE(TestProcedure4) {
//      __procedure(
//         __call(FailedSyncAction4),
//         __finally(__on_fail(__async(AsyncAction1))
//      ) procedure;
//   };
}