//
// Created by Darwin Yuan on 2020/6/17.
//

#include <nanobench/nanobench.h>
#include <trans-dsl/trans-dsl.h>
#include <event/impl/SimpleEventInfo.h>
#include "../ut/StupidTransactionContext.h"

StupidTransactionContext context;

#define SIMPLE_EVENT(n) \
const EV_NS::SimpleEventInfo eventInfo_## n{n}; \
const EV_NS::Event event_##n{eventInfo_##n}

SIMPLE_EVENT(1);
SIMPLE_EVENT(2);
SIMPLE_EVENT(3);
SIMPLE_EVENT(4);
SIMPLE_EVENT(5);
SIMPLE_EVENT(6);
SIMPLE_EVENT(7);
SIMPLE_EVENT(8);
SIMPLE_EVENT(9);


void func() {
   __procedure
   ( __sequential
        ( __wait(1)
        , __wait(2)
        , __wait(3)
        , __wait(4)
        , __wait(5)
        , __wait(6)),
     __finally( __sequential(__wait(7), __wait(8), __wait(9)))
   ) proc;

   assert(100 == proc.exec(context));
   assert(100 == proc.handleEvent(context, event_1));
   assert(100 == proc.handleEvent(context, event_2));
   assert(100 == proc.handleEvent(context, event_3));
   assert(100 == proc.handleEvent(context, event_4));
   assert(100 == proc.handleEvent(context, event_5));
   assert(100 == proc.handleEvent(context, event_6));
   assert(100 == proc.handleEvent(context, event_7));
   assert(100 == proc.handleEvent(context, event_8));
   assert(0 == proc.handleEvent(context, event_9));
}

int main() {
   ankerl::nanobench::Bench().epochs(1000).run("run-procedure", [&] {
      func();
   });

   return 0;
}