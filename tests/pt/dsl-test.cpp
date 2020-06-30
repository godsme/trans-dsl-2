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

namespace {
   using namespace TSL_NS;

   TEST_CASE("__time_guard __sequential") {
      StupidTransactionContext context;

      using Proc = __procedure
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

      auto proc = [&] {
         Proc proc;
         assert(Result::CONTINUE == proc.exec(context));
         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_1}));
         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_2}));
         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_3}));
         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_4}));
         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_5}));
         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_6}));
         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_7}));
         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_8}));
         assert(Result::SUCCESS  == proc.handleEvent(context, Event{se_9}));
      };

      SECTION("go") {
         ankerl::nanobench::Bench()
         .minEpochIterations(22072)
         .epochs(1000)
         .run("run-time-guard-seq", [&] {
            proc();
         });
      }
   };

   TEST_CASE("__sequential") {
      StupidTransactionContext context;

      using Proc = __procedure
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
         ankerl::nanobench::Bench().minEpochIterations(1818).epochs(1000).run("run-procedure", [&] {
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

      using Concurrent1 = __concurrent(ProcedureAction1, ProcedureAction2);
      using Concurrent2 = __concurrent(ProcedureAction1, ProcedureAction2, ProcedureAction3);

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};

      const Msg4 msg4{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo4{EV_MSG_4, msg4};

      auto runConcurrent = [&] {
         Concurrent1 action;

         assert(Result::CONTINUE == action.exec(context));
         assert(Result::CONTINUE == action.handleEvent(context, se_1));
         assert(Result::CONTINUE == action.handleEvent(context, se_2));
         assert(Result::CONTINUE == action.handleEvent(context, eventInfo1));
         assert(Result::SUCCESS == action.handleEvent(context, eventInfo2));
      };

      auto runConcurrent2 = [&] {
         Concurrent2 action;

         assert(Result::CONTINUE == action.exec(context));
         assert(Result::CONTINUE == action.handleEvent(context, se_1));
         assert(Result::CONTINUE == action.handleEvent(context, se_2));
         assert(Result::CONTINUE == action.handleEvent(context, se_3));
         assert(Result::CONTINUE == action.handleEvent(context, eventInfo1));
         assert(Result::CONTINUE == action.handleEvent(context, eventInfo4));
         assert(Result::SUCCESS == action.handleEvent(context, eventInfo2));
      };

      SECTION("size") {
         std::cout << "size = " << sizeof(Concurrent2) << std::endl;
      }

      SECTION("concurrent 1 performance") {
         ankerl::nanobench::Bench().epochs(1000).run("run-concurrent-1", [&] {
            runConcurrent();
         });
      }

      SECTION("concurrent 2 performance") {
         ankerl::nanobench::Bench().epochs(1000).run("run-concurrent-2", [&] {
            runConcurrent2();
         });
      }
   };

   TEST_CASE("concurrent") {
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

      using Concurrent2 = __concurrent(ProcedureAction1, ProcedureAction2, ProcedureAction3);

      using Proc = __procedure
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

      auto runSequential = [&] {
         Proc proc;
         assert(Result::CONTINUE == proc.exec(context));
         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_1}));
         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_2}));
         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_3}));
         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_4}));
         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_5}));
         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_6}));

         assert(Result::CONTINUE == proc.handleEvent(context, se_1));
         assert(Result::CONTINUE == proc.handleEvent(context, se_2));
         assert(Result::CONTINUE == proc.handleEvent(context, se_3));
         assert(Result::CONTINUE == proc.handleEvent(context, eventInfo1));
         assert(Result::CONTINUE == proc.handleEvent(context, eventInfo4));
         assert(Result::CONTINUE == proc.handleEvent(context, eventInfo2));

         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_7}));
         assert(Result::CONTINUE == proc.handleEvent(context, Event{se_8}));
         assert(Result::SUCCESS == proc.handleEvent(context, Event{se_9}));
      };

      SECTION("size") {
         std::cout << sizeof(Proc) << std::endl;
      }

      SECTION("performance") {
         ankerl::nanobench::Bench()
         .minEpochIterations(1322)
         .epochs(1000)
         .run("run-sequential", [&] {
            runSequential();
         });
      }
   }
}
