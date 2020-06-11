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

      const Msg1 msg3{ 30 };
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

      TEST("call exec -> event1 -> event3 -> event1, should return FAILED") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event1));
         ASSERT_EQ(Result::CONTINUE, procedure.handleEvent(context, event3));
         ASSERT_EQ(Result::FAILED, procedure.handleEvent(context, event1));
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

      const Msg1 msg3{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo3{EV_MSG_3, msg3};
      TSL_NS::Event event3{eventInfo3};

      TEST("exec should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
      }

      TEST("call exec again, should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, procedure.exec(context));
         ASSERT_EQ(Result::FATAL_BUG, procedure.exec(context));
      }

      TEST("call exec -> handleEvent(event1), should return UNKNOWN_EVENT") {
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

}