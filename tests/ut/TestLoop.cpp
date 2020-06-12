//
// Created by Darwin Yuan on 2020/6/12.
//

#include <cctest/cctest.h>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/OptionalHelper.h>
#include <trans-dsl/sched/helper/LoopHelper.h>
#include <trans-dsl/sched/helper/SyncActionHelper.h>
#include <iostream>

namespace {
   using namespace TSL_NS;

   bool is_true(const TransactionInfo&) { return true; }

   struct IsTrue {
      bool operator()(const TransactionInfo&) {
         return true;
      }

      int i = 0;
   };

   FIXTURE(TestLoop) {
     __loop(__async(AsyncAction1), __break_if(IsTrue, Result::SUCCESS), __sync(SyncAction3)) a;

     TEST("is") {
        auto i = details::LoopActionType::ACTION;
        a.get(0, i);
        ASSERT_EQ(details::LoopActionType::ACTION, i);
        a.get(1, i);
        ASSERT_EQ(details::LoopActionType::BREAK_PRED, i);
        a.get(2, i);
        ASSERT_EQ(details::LoopActionType::ACTION, i);
     }
   };
}