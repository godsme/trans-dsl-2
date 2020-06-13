//
// Created by Darwin Yuan on 2020/6/13.
//

#include <cctest/cctest.h>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/OptionalHelper.h>
#include <trans-dsl/sched/helper/SyncActionHelper.h>
#include <iostream>
#include <trans-dsl/sched/helper/ProcedureHelper.h>
#include <trans-dsl/sched/helper/SequentialHelper.h>
#include <trans-dsl/sched/helper/ConcurrentHelper.h>
#include <array>

namespace {
   using namespace TSL_NS;


   FIXTURE(TestConcurrent) {
      __concurrent(__async(AsyncAction1), __async(AsyncAction2)) action;

      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      TEST("event1 -> event2 should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event2));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event1));
      }

      TEST("stop should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::SUCCESS, action.stop(context));
      }

      TEST("after stopped, start again should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::SUCCESS, action.stop(context));
         ASSERT_EQ(Result::FATAL_BUG, action.exec(context));
      }

      TEST("after killed, start again should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         action.kill(context);
         ASSERT_EQ(Result::FATAL_BUG, action.exec(context));
      }

      TEST("after stopped, handleEvent again should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::SUCCESS, action.stop(context));
         ASSERT_EQ(Result::FATAL_BUG, action.handleEvent(context, event1));
      }

      TEST("after killed, handleEvent again should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         action.kill(context);
         ASSERT_EQ(Result::FATAL_BUG, action.handleEvent(context, event1));
      }

      TEST("after stopped, handleEvent again should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::SUCCESS, action.stop(context));
         ASSERT_EQ(Result::FATAL_BUG, action.handleEvent(context, event2));
      }

      TEST("event2 -> event1 should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event1));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event2));
      }
   };

   FIXTURE(TestConcurrent1) {
      __concurrent(__sync(SyncAction1), __sync(SyncAction1)) action;

      StupidTransactionContext context{};

      TEST("exec should return SUCCESS") {
         std::cout << sizeof(action) << std::endl;
         ASSERT_EQ(Result::SUCCESS, action.exec(context));
      }
   };

   FIXTURE(TestConcurrent2) {
      __concurrent(__async(AsyncAction1), __sync(FailedSyncAction4)) action;

      StupidTransactionContext context{};

      TEST("exec should return FAILED") {
         ASSERT_EQ(Result::FAILED, action.exec(context));
      }
   };

   using ProcedureAction =
   __procedure(__async(AsyncAction4), __finally(__on_status(Result::FAILED, __async(AsyncAction2))));

   using ProcedureAction1 =
   __procedure(__async(AsyncAction4), __finally(__async(AsyncAction2)));

   FIXTURE(TestConcurrent3) {
      __concurrent(ProcedureAction1, __sync(FailedSyncAction4)) action;

      StupidTransactionContext context{};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      TEST("exec should return FAILED") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::FAILED, action.handleEvent(context, event2));
      }

      TEST("exec -> stop should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.stop(context));
      }

      TEST("exec should report Failure") {
         ASSERT_EQ(Result::SUCCESS, context.getStatus());
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::FAILED, context.getStatus());
      }
   };

   FIXTURE(TestConcurrent4) {
      __concurrent(__sync(FailedSyncAction4), ProcedureAction) action;

      StupidTransactionContext context{};

      TEST("exec should return FAILED") {
         ASSERT_EQ(Result::FAILED, action.exec(context));
      }
   };

   FIXTURE(TestConcurrent5) {
      __concurrent(__async(FailedAsyncAction3), ProcedureAction) action;

      StupidTransactionContext context{};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      const Msg3 msg3{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo3{EV_MSG_3, msg3};
      TSL_NS::Event event3{eventInfo3};

      TEST("exec should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
      }

      TEST("event3 should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event3));
      }

      TEST("event3 should report failture to runtime context") {
         ASSERT_EQ(Result::SUCCESS, context.getStatus());
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::SUCCESS, context.getStatus());
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event3));
         ASSERT_EQ(Result::FAILED, context.getStatus());
      }

      TEST("event3 -> event2 should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event3));
         ASSERT_EQ(Result::FAILED, action.handleEvent(context, event2));
      }

      TEST("stop should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         context.RuntimeContext::reportFailure(Result::FAILED);
         ASSERT_EQ(Result::CONTINUE, action.stop(context));
      }

      TEST("if not error on runtime-context, stop should return FORCE_STOPPED") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::FORCE_STOPPED, action.stop(context));
      }

      TEST("after stop, event2 should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         context.RuntimeContext::reportFailure(Result::FAILED);
         ASSERT_EQ(Result::CONTINUE, action.stop(context));
         ASSERT_EQ(Result::FAILED, action.handleEvent(context, event2));
      }

      TEST("after stop, event3 should return UNKNOWN_EVENT") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         context.RuntimeContext::reportFailure(Result::FAILED);
         ASSERT_EQ(Result::CONTINUE, action.stop(context));
         ASSERT_EQ(Result::UNKNOWN_EVENT, action.handleEvent(context, event3));
      }

      TEST("after stop, event2 should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         context.RuntimeContext::reportFailure(Result::OUT_OF_SCOPE);
         ASSERT_EQ(Result::OUT_OF_SCOPE, action.stop(context));
      }
   };

   using ProcedureAction2 =
   __procedure(
      __async(AsyncAction4),
      __finally(__on_status(Result::FORCE_STOPPED, __async(AsyncAction2))));

   FIXTURE(TestConcurrent6) {
      __concurrent(__async(FailedAsyncAction3), ProcedureAction2) action;

      StupidTransactionContext context{};

      TEST("after stop, event3 should return UNKNOWN_EVENT") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.stop(context));
      }
   };

}
