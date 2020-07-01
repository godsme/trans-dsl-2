//
// Created by Darwin Yuan on 2020/6/17.
//

#include <nanobench.h>
#include <trans-dsl/trans-dsl.h>
#include <trans-dsl/action/SimpleAsyncAction.h>
#include <event/impl/ConsecutiveEventInfo.h>
#include <event/impl/SimpleEventInfo.h>
#include <event/concept/Event.h>
#include "../tests/ut/StupidTransactionContext.h"
#include <iostream>

using namespace tsl;

StupidTransactionContext context;

#define SIMPLE_EVENT(n) \
const EV_NS::SimpleEventInfo ev_## n{n}

SIMPLE_EVENT(1);
SIMPLE_EVENT(2);
SIMPLE_EVENT(3);
SIMPLE_EVENT(4);
SIMPLE_EVENT(5);
SIMPLE_EVENT(6);
SIMPLE_EVENT(7);
SIMPLE_EVENT(8);
SIMPLE_EVENT(9);

using Seq1 = __procedure
( __sequential
     ( __wait(1)
        , __wait(2)
        , __wait(3)
        , __wait(4)
        , __wait(5)
        , __wait(6)),
  __finally( __sequential(__wait(7), __wait(8), __wait(9)))
);

void func() {
   Seq1 proc;

   proc.exec(context);
   proc.handleEvent(context, ev_1);
   proc.handleEvent(context, ev_2);
   proc.handleEvent(context, ev_3);
   proc.handleEvent(context, ev_4);
   proc.handleEvent(context, ev_5);
   proc.handleEvent(context, ev_6);
   proc.handleEvent(context, ev_7);
   proc.handleEvent(context, ev_8);
   proc.handleEvent(context, ev_9);
}

using Seq2 = __procedure
( __time_guard(1,
   __sequential
     ( __wait(1)
        , __wait(2)
        , __wait(3)
        , __wait(4)
        , __wait(5)
        , __wait(6))),
  __finally( __sequential(__wait(7), __wait(8), __wait(9)))
);

void func0() {
   Seq2 proc;

   proc.exec(context);
   proc.handleEvent(context, ev_1);
   proc.handleEvent(context, ev_2);
   proc.handleEvent(context, ev_3);
   proc.handleEvent(context, ev_4);
   proc.handleEvent(context, ev_5);
   proc.handleEvent(context, ev_6);
   proc.handleEvent(context, ev_7);
   proc.handleEvent(context, ev_8);
   proc.handleEvent(context, ev_9);
}

const EV_NS::EventId EV_MSG_1 = 1000;
const EV_NS::EventId EV_MSG_2 = 2000;
const EV_NS::EventId EV_MSG_3 = 2000;
const EV_NS::EventId EV_MSG_4 = 4000;
struct Msg1 {
   int a = 0;
   int b = 0;
};

struct Msg2 {
   int a = 0;
};

struct Msg3 {
   int a = 0;
};

struct Msg4 {
   int a = 0;
};


DEF_SIMPLE_ASYNC_ACTION(AsyncAction1) {
      auto exec(TSL_NS::TransactionInfo const&) -> TSL_NS::Status {
         return WAIN_ON(EV_MSG_1, handleMsg1);
      }

      DEF_INLINE_EVENT_HANDLER(handleMsg1, Msg1) {
         return TSL_NS::Result::SUCCESS;
      }
   };

   DEF_SIMPLE_ASYNC_ACTION(AsyncAction2) {
      auto exec(TSL_NS::TransactionInfo const&) ->TSL_NS::Status {
         return WAIN_ON(EV_MSG_2, handleMsg2);
      }

      DEF_INLINE_EVENT_HANDLER(handleMsg2, Msg2) {
         return TSL_NS::Result::SUCCESS;
      }
   };

   DEF_SIMPLE_ASYNC_ACTION(FailedAsyncAction3) {
      auto exec(TSL_NS::TransactionInfo const&) -> TSL_NS::Status {
         return WAIN_ON(EV_MSG_3, handleMsg3);
      }

      DEF_INLINE_EVENT_HANDLER(handleMsg3, Msg3) {
         return TSL_NS::Result::FAILED;
      }
   };

   DEF_SIMPLE_ASYNC_ACTION(AsyncAction4) {
      auto exec(TSL_NS::TransactionInfo const&) -> TSL_NS::Status {
         return WAIN_ON(EV_MSG_4, handleMsg4);
      }

      DEF_INLINE_EVENT_HANDLER(handleMsg4, Msg4) {
         return TSL_NS::Result::SUCCESS;
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




const Msg1 msg1{ 10, 20 };
const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};

const Msg2 msg2{ 30 };
const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};

const Msg4 msg4{ 30 };
const EV_NS::ConsecutiveEventInfo eventInfo4{EV_MSG_4, msg4};

using Con1 = __concurrent(ProcedureAction1, ProcedureAction2);
void func1() {
    Con1 action;

   action.exec(context);
   action.handleEvent(context, ev_1);
   action.handleEvent(context, ev_2);
   action.handleEvent(context, eventInfo1);
   action.handleEvent(context, eventInfo2);

//   assert(CONTINUE == action.exec(context));
//   assert(CONTINUE == action.handleEvent(context, ev_1));
//   assert(CONTINUE == action.handleEvent(context, ev_2));
//   assert(CONTINUE == action.handleEvent(context, eventInfo1));
//   assert(SUCCESS  == action.handleEvent(context, eventInfo2));
}

using Con2 = __concurrent(ProcedureAction1, ProcedureAction2, ProcedureAction3);
void func2() {
    Con2 action;

   action.exec(context);
   action.handleEvent(context, ev_1);
   action.handleEvent(context, ev_2);
   action.handleEvent(context, ev_3);
   action.handleEvent(context, eventInfo1);
   action.handleEvent(context, eventInfo4);
   action.handleEvent(context, eventInfo2);

//   assert(CONTINUE == action.exec(context));
//   assert(CONTINUE == action.handleEvent(context, ev_1));
//   assert(CONTINUE == action.handleEvent(context, ev_2));
//   assert(CONTINUE == action.handleEvent(context, ev_3));
//   assert(CONTINUE == action.handleEvent(context, eventInfo1));
//   assert(CONTINUE == action.handleEvent(context, eventInfo4));
//   assert(SUCCESS  == action.handleEvent(context, eventInfo2));
}

using Proc2 = __procedure
( __sequential
     ( __wait(1)
     , __wait(2)
     , __wait(3)
     , __wait(4)
     , __wait(5)
     , __wait(6)
     , Con2),
  __finally(__sequential(__wait(7), __wait(8), __wait(9)))
);

template <typename P>
void func3() {
   P proc;
   assert(CONTINUE == proc.exec(context));
   assert(CONTINUE == proc.handleEvent(context, ev_1));
   assert(CONTINUE == proc.handleEvent(context, ev_2));
   assert(CONTINUE == proc.handleEvent(context, ev_3));
   assert(CONTINUE == proc.handleEvent(context, ev_4));
   assert(CONTINUE == proc.handleEvent(context, ev_5));
   assert(CONTINUE == proc.handleEvent(context, ev_6));
   assert(CONTINUE == proc.handleEvent(context, ev_1));
   assert(CONTINUE == proc.handleEvent(context, ev_2));
   assert(CONTINUE == proc.handleEvent(context, ev_3));
   assert(CONTINUE == proc.handleEvent(context, eventInfo1));
   assert(CONTINUE == proc.handleEvent(context, eventInfo4));
   assert(CONTINUE == proc.handleEvent(context, eventInfo2));
   assert(CONTINUE == proc.handleEvent(context, ev_7));
   assert(CONTINUE == proc.handleEvent(context, ev_8));
   assert(SUCCESS  == proc.handleEvent(context, ev_9));
}

using Proc3 = 
__time_guard(1, 
  __procedure( 
    __sequential
     ( __wait(1)
     , __wait(2)
     , __wait(3)
     , __wait(4)
     , __wait(5)
     , __wait(6)
     , Con2),
  __finally(__sequential(__wait(7), __wait(8), __wait(9))))
);

using Proc4 = __procedure
( __sequential( __sequential
     ( __wait(1)
     , __wait(2)
     , __wait(3)
     , __wait(4)
     , __wait(5)
     , __wait(6))
     , Con2),
  __finally(__sequential(__wait(7), __wait(8), __wait(9)))
);

int main() {
   ankerl::nanobench::Bench().minEpochIterations(11174).epochs(1000).run("simple seq", [&] {
      func();
   });
   ankerl::nanobench::Bench().minEpochIterations(11).epochs(1000).run("2 concurrent", [&] {
      func1();
   });

   ankerl::nanobench::Bench().minEpochIterations(1213).epochs(1000).run("3 concurrent", [&] {
      func2();
   });

   ankerl::nanobench::Bench().minEpochIterations(195).epochs(1000).run("3-concurrent-seq", [&] {
      func3<Proc2>();
   });

   ankerl::nanobench::Bench().minEpochIterations(195).epochs(1000).run("timer-guard-concurrent-seq", [&] {
      func3<Proc3>();
   });

   ankerl::nanobench::Bench().minEpochIterations(1213).epochs(1000).run("timer-guard-simple-seq", [&] {
      func0();
   });

   ankerl::nanobench::Bench().minEpochIterations(195).epochs(1000).run("concurrent-seq-seq", [&] {
      func3<Proc4>();
   });

   return 0;
}
