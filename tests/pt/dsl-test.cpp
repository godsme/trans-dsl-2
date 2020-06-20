//
// Created by Darwin Yuan on 2020/6/17.
//

#include <nanobench.h>
#include <cctest/cctest.h>
#include "../ut/StupidTransactionContext.h"
#include "../ut/SimpleActionsDefs.h"
#include <trans-dsl/trans-dsl.h>
#include <iostream>


#define SIMPLE_EVENT(n) \
const EV_NS::SimpleEventInfo se_## n{n}

SIMPLE_EVENT(1);
SIMPLE_EVENT(2);
SIMPLE_EVENT(3);
SIMPLE_EVENT(4);
SIMPLE_EVENT(5);
SIMPLE_EVENT(6);
SIMPLE_EVENT(7);
SIMPLE_EVENT(8);
SIMPLE_EVENT(9);

namespace {
   using namespace TSL_NS;



   FIXTURE(TestSequential) {
      StupidTransactionContext context;
      using Proc = __procedure
      (__sequential
          ( __wait(1)
             , __wait(2)
             , __wait(3)
             , __wait(4)
             , __wait(5)
             , __wait(6)),
       __finally(__sequential(__wait(7), __wait(8), __wait(9)))
      );

      void runSequential() {
         Proc proc;
         ASSERT_EQ(Result::CONTINUE, proc.exec(context));
         ASSERT_EQ(Result::CONTINUE, proc.handleEvent(context, Event{se_1}));
         ASSERT_EQ(Result::CONTINUE, proc.handleEvent(context, Event{se_2}));
         ASSERT_EQ(Result::CONTINUE, proc.handleEvent(context, Event{se_3}));
         ASSERT_EQ(Result::CONTINUE, proc.handleEvent(context, Event{se_4}));
         ASSERT_EQ(Result::CONTINUE, proc.handleEvent(context, Event{se_5}));
         ASSERT_EQ(Result::CONTINUE, proc.handleEvent(context, Event{se_6}));
         ASSERT_EQ(Result::CONTINUE, proc.handleEvent(context, Event{se_7}));
         ASSERT_EQ(Result::CONTINUE, proc.handleEvent(context, Event{se_8}));
         ASSERT_EQ(Result::SUCCESS,  proc.handleEvent(context, Event{se_9}));
      }

      TEST("test") {
         ankerl::nanobench::Bench().epochs(1000).run("run-procedure", [&] {
            runSequential();
         });
      }
   };

   FIXTURE(TestConcurrent) {
      StupidTransactionContext context;

      using ProcedureAction1 =
      __procedure(
         __wait(1),
         __finally(__asyn(AsyncAction2)));

      using ProcedureAction2 =
      __procedure(
         __wait(2),
         __finally(__asyn(AsyncAction1)));

      using Concurrent = __concurrent(ProcedureAction1, ProcedureAction2);

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};

      void runConcurrent() {
         Concurrent action;

         ASSERT_EQ(Result::CONTINUE, action.exec(context));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, Event{se_1}));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, Event{se_2}));
         ASSERT_EQ(Result::CONTINUE, action.handleEvent(context, Event{eventInfo1}));
         ASSERT_EQ(Result::SUCCESS,  action.handleEvent(context, Event{eventInfo2}));
      }

      TEST("functional") {
         runConcurrent();
      }

      TEST("performance") {
         ankerl::nanobench::Bench().epochs(1000).run("run-concurrent", [&] {
            runConcurrent();
         });
      }
   };
}
