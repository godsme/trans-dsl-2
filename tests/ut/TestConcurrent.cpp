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
      __concurrent(__asyn(AsyncAction1), __asyn(AsyncAction2)) action;

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

      TEST("stop should return FORCE_STOPPED") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::FORCE_STOPPED, action.stop(context, Result::OUT_OF_SCOPE));
      }

      TEST("after stopped, start again should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::FORCE_STOPPED, action.stop(context, Result::OUT_OF_SCOPE));
         ASSERT_EQ(Result::FATAL_BUG, action.exec(context));
      }

      TEST("after killed, start again should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         action.kill(context, Result::DUPTID);
         ASSERT_EQ(Result::FATAL_BUG, action.exec(context));
      }

      TEST("after stopped, handleEvent again should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::FORCE_STOPPED, action.stop(context, Result::OUT_OF_SCOPE));
         ASSERT_EQ(Result::FATAL_BUG, action.handleEvent(context, event1));
      }

      TEST("after killed, handleEvent again should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         action.kill(context, Result::DUPTID);
         ASSERT_EQ(Result::FATAL_BUG, action.handleEvent(context, event1));
      }

      TEST("after stopped, handleEvent again should return FATAL_BUG") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::FORCE_STOPPED, action.stop(context, Result::OUT_OF_SCOPE));
         ASSERT_EQ(Result::FATAL_BUG, action.handleEvent(context, event2));
      }

      TEST("event2 -> event1 should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event1));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event2));
      }
   };

   FIXTURE(TestConcurrent1) {
      __concurrent(__syn(SyncAction1), __syn(SyncAction1)) action;

      StupidTransactionContext context{};

      TEST("exec should return SUCCESS") {
         std::cout << sizeof(action) << std::endl;
         ASSERT_EQ(Result::SUCCESS, action.exec(context));
      }
   };

   FIXTURE(TestConcurrent2) {
      __concurrent(__asyn(AsyncAction1), __syn(FailedSyncAction4)) action;

      StupidTransactionContext context{};

      TEST("exec should return FAILED") {
         ASSERT_EQ(Result::FAILED, action.exec(context));
      }
   };

   using ProcedureAction =
   __procedure(__asyn(AsyncAction4), __finally(__on_status(Result::FORCE_STOPPED, __asyn(AsyncAction2))));

   using ProcedureAction1 =
   __procedure(__asyn(AsyncAction4), __finally(__asyn(AsyncAction2)));

   FIXTURE(TestConcurrent3) {
      __concurrent(ProcedureAction1, __syn(FailedSyncAction4)) action;

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
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::OUT_OF_SCOPE));
      }

      TEST("exec should report Failure") {
         ASSERT_EQ(Result::SUCCESS, context.getRuntimeEnvStatus());
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::FAILED, context.getRuntimeEnvStatus());
      }
   };

   FIXTURE(TestConcurrent4) {
      __concurrent(__syn(FailedSyncAction4), ProcedureAction) action;

      StupidTransactionContext context{};

      TEST("exec should return FAILED") {
         ASSERT_EQ(Result::FAILED, action.exec(context));
      }
   };

   FIXTURE(TestConcurrent5) {
      __concurrent(__asyn(FailedAsyncAction3), ProcedureAction) action;

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
         ASSERT_EQ(Result::SUCCESS, context.getRuntimeEnvStatus());
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::SUCCESS, context.getRuntimeEnvStatus());
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event3));
         ASSERT_EQ(Result::FAILED, context.getRuntimeEnvStatus());
      }

      TEST("event3 -> event2 should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event3));
         ASSERT_EQ(Result::FAILED, action.handleEvent(context, event2));
      }

      TEST("stop should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         context.reportFailure(Result::FAILED);
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::OUT_OF_SCOPE));
      }

      TEST("if not error on runtime-context, stop should return FORCE_STOPPED") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::OUT_OF_SCOPE));
         ASSERT_EQ(Result::FORCE_STOPPED, action.handleEvent(context, event2));
      }

      TEST("after stop, event2 should return FORCE_STOPPED") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::OUT_OF_SCOPE));
         ASSERT_EQ(Result::FORCE_STOPPED, action.handleEvent(context, event2));
      }

      TEST("after stop, event3 should return UNKNOWN_EVENT") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         context.reportFailure(Result::FAILED);
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::OUT_OF_SCOPE));
         ASSERT_EQ(Result::UNKNOWN_EVENT, action.handleEvent(context, event3));
      }

      TEST("after stop, stop again should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::TIMEDOUT));
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::TIMEDOUT));
      }
   };

   using ProcedureAction2 =
   __procedure(
      __asyn(AsyncAction4),
      __finally(__on_status(Result::FORCE_STOPPED, __asyn(AsyncAction2))));

   FIXTURE(TestConcurrent6) {
      __concurrent(__asyn(FailedAsyncAction3), ProcedureAction2) action;

      StupidTransactionContext context{};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      TEST("after stop, event3 should return UNKNOWN_EVENT") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::OUT_OF_SCOPE));
         ASSERT_EQ(Result::FORCE_STOPPED, action.handleEvent(context, event2));
      }
   };

   using ProcedureAction3 =
   __procedure(
      __asyn(FailedAsyncAction3),
      __finally(__asyn(AsyncAction2)));

   FIXTURE(TestConcurrent7) {
      __concurrent(__asyn(AsyncAction1), ProcedureAction3) action;

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

      TEST("should not process AsyncAction1 if event3 arrives first") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event3));
         ASSERT_EQ(Result::UNKNOWN_EVENT, action.handleEvent(context, event1));
         ASSERT_EQ(Result::FAILED, action.handleEvent(context, event2));
      }
   };

   using ProcedureAction4 =
   __procedure(
      __syn(SyncAction3),
      __finally(__asyn(AsyncAction2)));

   using ProcedureAction5 =
   __procedure(
      __syn(SyncAction2),
      __finally(__asyn(AsyncAction1)));

   FIXTURE(TestConcurrent8) {
      __concurrent(ProcedureAction4, ProcedureAction5) action;

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      StupidTransactionContext context{};

      TEST("stop doesn't work") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::OUT_OF_SCOPE));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event1));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event2));
      }

      TEST("stop doesn't work 2") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event1));
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::OUT_OF_SCOPE));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event2));
      }

      TEST("stop doesn't work in any order") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::OUT_OF_SCOPE));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event2));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event1));
      }

      TEST("stop doesn't work in any order 2") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event2));
         ASSERT_EQ(Result::CONTINUE, action.stop(context, Result::OUT_OF_SCOPE));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event1));
      }

      TEST("normal-case") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, event1));
         ASSERT_EQ(Result::SUCCESS, action.handleEvent(context, event2));
      }
   };
}
