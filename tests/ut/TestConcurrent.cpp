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

   using ProcedureAction =
      __procedure(__async(AsyncAction4), __finally(__async(AsyncAction2)));

   FIXTURE(TestConcurrent) {
      __concurrent(__async(AsyncAction1), __async(AsyncAction2)) action;

      StupidTransactionContext context{};

      TEST("exec should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
      }
   };

   FIXTURE(TestConcurrent1) {
      __concurrent(__sync(SyncAction1), __sync(SyncAction1)) action;

      StupidTransactionContext context{};

      TEST("exec should return CONTINUE") {
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

   FIXTURE(TestConcurrent3) {
      __concurrent(ProcedureAction, __sync(FailedSyncAction4)) action;

      StupidTransactionContext context{};

      TEST("exec should return FAILED") {
         ASSERT_EQ(Result::CONTINUE, action.exec(context));
      }
   };
}
