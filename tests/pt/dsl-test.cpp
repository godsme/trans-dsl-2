//
// Created by Darwin Yuan on 2020/6/17.
//

#include <nanobench.h>
//#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch.hpp>
#include "../ut/StupidTransactionContext.h"
#include "../ut/SimpleActionsDefs.h"
#include <trans-dsl/trans-dsl.h>
#include <iostream>


#define SIMPLE_EVENT(n) \
EV_NS::SimpleEventInfo const se_## n{n}

SIMPLE_EVENT(1);
SIMPLE_EVENT(2);
SIMPLE_EVENT(3);
SIMPLE_EVENT(4);
SIMPLE_EVENT(5);
SIMPLE_EVENT(6);
SIMPLE_EVENT(7);
SIMPLE_EVENT(8);
SIMPLE_EVENT(9);

#define EXPECT_EQ_1(expected, ...) do { \
   Status result = __VA_ARGS__; \
   total &= result;   \
   assert(expected == result); \
} while(0)

namespace {
   using namespace TSL_NS;

   using Proc1 = __def_procedure
   ( __time_guard(TIMER_1
      , __sequential
                     ( __wait(1)
                        , __wait(2)
                        , __wait(3)
                        , __wait(4)
                        , __wait(5)
                        , __wait(6))),
     __finally(__sequential(__wait(7), __wait(8), __wait(9)))
   );



   TEST_CASE("__time_guard __sequential") {
      StupidTransactionContext context;

      auto func1 = [&] {
         Proc1 proc;
         Status total = 0;
         EXPECT_EQ_1(Result::CONTINUE, proc.exec(context));
         EXPECT_EQ_1(Result::CONTINUE, proc.handleEvent(context, Event{se_1}));
         EXPECT_EQ_1(Result::CONTINUE, proc.handleEvent(context, Event{se_2}));
         EXPECT_EQ_1(Result::CONTINUE, proc.handleEvent(context, Event{se_3}));
         EXPECT_EQ_1(Result::CONTINUE, proc.handleEvent(context, Event{se_4}));
         EXPECT_EQ_1(Result::CONTINUE, proc.handleEvent(context, Event{se_5}));
         EXPECT_EQ_1(Result::CONTINUE, proc.handleEvent(context, Event{se_6}));
         EXPECT_EQ_1(Result::CONTINUE, proc.handleEvent(context, Event{se_7}));
         EXPECT_EQ_1(Result::CONTINUE, proc.handleEvent(context, Event{se_8}));
         EXPECT_EQ_1(Result::SUCCESS,  proc.handleEvent(context, Event{se_9}));
         return total;
      };

      SECTION("go") {
         ankerl::nanobench::Bench()
         .minEpochIterations(100)
         .epochs(1000)
         .run("run-time-guard-seq", [&] {
            auto i = func1();
            ankerl::nanobench::doNotOptimizeAway(i);
         });
      }
   };

   TEST_CASE("__sequential") {
      StupidTransactionContext context;

      using Proc = __def_procedure
      ( __sequential
          ( __wait(1)
             , __wait(2)
             , __wait(3)
             , __wait(4)
             , __wait(5)
             , __wait(6)),
        __finally(__sequential(__wait(7), __wait(8), __wait(9)))
      );

      auto runSequential = [&] {
         Proc proc;
         proc.exec(context);
         proc.handleEvent(context, Event{se_1});
         proc.handleEvent(context, Event{se_2});
         proc.handleEvent(context, Event{se_3});
         proc.handleEvent(context, Event{se_4});
         proc.handleEvent(context, Event{se_5});
         proc.handleEvent(context, Event{se_6});
         proc.handleEvent(context, Event{se_7});
         proc.handleEvent(context, Event{se_8});
         proc.handleEvent(context, Event{se_9});
      };

      SECTION("test") {
         ankerl::nanobench::Bench()
         .minEpochIterations(224)
         .epochs(1000).run("run-procedure-2", [&] {
            runSequential();
         });
      }
   };

   TEST_CASE("__concurrent") {
      StupidTransactionContext context;

      using ProcedureAction1 =
      __procedure(
         __wait(1),
         __finally(__asyn(AsyncAction2)));

      using ProcedureAction2 =
      __procedure(
         __wait(2),
         __finally(__asyn(AsyncAction1)));

      using ProcedureAction3 =
      __procedure(
         __wait(3),
         __finally(__asyn(AsyncAction4)));

      using Concurrent1 = __concurrent_t(ProcedureAction1, ProcedureAction2);
      using Concurrent2 = __concurrent_t(ProcedureAction1, ProcedureAction2, ProcedureAction3);

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};

      const Msg4 msg4{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo4{EV_MSG_4, msg4};

      auto runConcurrent = [&] {
         Concurrent1 action;
         Status total = 0;
         EXPECT_EQ_1(Result::CONTINUE, action.exec(context));
         EXPECT_EQ_1(Result::CONTINUE, action.handleEvent(context, se_1));
         EXPECT_EQ_1(Result::CONTINUE, action.handleEvent(context, se_2));
         EXPECT_EQ_1(Result::CONTINUE, action.handleEvent(context, eventInfo1));
         EXPECT_EQ_1(Result::SUCCESS, action.handleEvent(context, eventInfo2));

         return total;
      };

      auto runConcurrent2 = [&] {
         Concurrent2 action;
         Status total = 0;

         EXPECT_EQ_1(Result::CONTINUE, action.exec(context));
         EXPECT_EQ_1(Result::CONTINUE, action.handleEvent(context, se_1));
         EXPECT_EQ_1(Result::CONTINUE, action.handleEvent(context, se_2));
         EXPECT_EQ_1(Result::CONTINUE, action.handleEvent(context, se_3));
         EXPECT_EQ_1(Result::CONTINUE, action.handleEvent(context, eventInfo1));
         EXPECT_EQ_1(Result::CONTINUE, action.handleEvent(context, eventInfo4));
         EXPECT_EQ_1(Result::SUCCESS, action.handleEvent(context, eventInfo2));
         return total;
      };

      SECTION("concurrent 1 performance") {
         ankerl::nanobench::Bench().epochs(1000).run("run-concurrent-1", [&] {
            ankerl::nanobench::doNotOptimizeAway(runConcurrent);
            runConcurrent();
         });
      }

      SECTION("concurrent 2 performance") {
         ankerl::nanobench::Bench().epochs(1000).run("run-concurrent-2", [&] {
            ankerl::nanobench::doNotOptimizeAway(runConcurrent2);
            runConcurrent2();
         });
      }
   };

   using ProcedureAction1 =
   __procedure(
      __wait(1),
      __finally(__asyn(AsyncAction2)));

   using ProcedureAction2 =
   __procedure(
      __wait(2),
      __finally(__asyn(AsyncAction1)));

   using ProcedureAction3 =
   __procedure(
      __wait(3),
      __finally(__asyn(AsyncAction4)));

   using Concurrent2 = __concurrent(ProcedureAction1, ProcedureAction2, ProcedureAction3);

   using Proc = __def_procedure
   ( __sequential
        ( __wait(1)
           , __wait(2)
           , __wait(3)
           , __wait(4)
           , __wait(5)
           , __wait(6)
           , Concurrent2),
     __finally(__sequential(__wait(7), __wait(8), __wait(9)))
   );

   const Msg1 msg1{ 10, 20 };
   const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};

   const Msg2 msg2{ 30 };
   const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};

   const Msg4 msg4{ 30 };
   const EV_NS::ConsecutiveEventInfo eventInfo4{EV_MSG_4, msg4};



   TEST_CASE("concurrent") {
      StupidTransactionContext context;
      auto func4 = [&] {
         Proc proc;
         Status total = 0;
         EXPECT_EQ_1(Result::CONTINUE,proc.exec(context));
         EXPECT_EQ_1(Result::CONTINUE,proc.handleEvent(context, Event{se_1}));
         EXPECT_EQ_1(Result::CONTINUE,proc.handleEvent(context, Event{se_2}));
         EXPECT_EQ_1(Result::CONTINUE,proc.handleEvent(context, Event{se_3}));
         EXPECT_EQ_1(Result::CONTINUE,proc.handleEvent(context, Event{se_4}));
         EXPECT_EQ_1(Result::CONTINUE,proc.handleEvent(context, Event{se_5}));
         EXPECT_EQ_1(Result::CONTINUE,proc.handleEvent(context, Event{se_6}));

         EXPECT_EQ_1(Result::CONTINUE,proc.handleEvent(context, se_1));
         EXPECT_EQ_1(Result::CONTINUE,proc.handleEvent(context, se_2));
         EXPECT_EQ_1(Result::CONTINUE,proc.handleEvent(context, se_3));
         EXPECT_EQ_1(Result::CONTINUE,proc.handleEvent(context, eventInfo1));
         EXPECT_EQ_1(Result::CONTINUE,proc.handleEvent(context, eventInfo4));
         EXPECT_EQ_1(Result::CONTINUE,proc.handleEvent(context, eventInfo2));

         EXPECT_EQ_1(Result::CONTINUE,proc.handleEvent(context, Event{se_7}));
         EXPECT_EQ_1(Result::CONTINUE,proc.handleEvent(context, Event{se_8}));
         EXPECT_EQ_1(Result::SUCCESS, proc.handleEvent(context, Event{se_9}));

         return total;
      };

      SECTION("performance") {
         ankerl::nanobench::Bench()
         .minEpochIterations(100)
         .epochs(1000)
         .run("run-sequential", [&] {
            auto i = func4();
            ankerl::nanobench::doNotOptimizeAway(i);
         });
      }
   }

   TEST_CASE("__multi_thread") {
      using Con2 = __concurrent(ProcedureAction1, ProcedureAction2, ProcedureAction3);
      using Proc8 = __def_multi_thread(__procedure
      ( __sequential
           ( __sequential
               ( __wait(1)
               , __wait(2)
               , __fork(1, Con2)
               , __wait(3)
               , __wait(4)
               , __wait(5)
               , __wait(6))
           , __join()),
        __finally(__sequential(__wait(7), __wait(8), __wait(9)))
      ));

      StupidTransactionContext context;
      Proc8 action;

      REQUIRE(Result::CONTINUE == action.exec(context));
   }
}
