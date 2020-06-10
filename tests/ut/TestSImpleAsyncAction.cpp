//
// Created by Darwin Yuan on 2020/6/10.
//

#include <cctest/cctest.h>
#include <trans-dsl/action/SimpleAsyncAction.h>
#include <event/impl/ConsecutiveEventInfo.h>
#include <event/impl/SimpleEventInfo.h>
#include <event/concept/Event.h>
#include "StupidTransactionContext.h"

namespace {
   using namespace TSL_NS;

   const EventId EV_MSG_1 = 1;
   struct Msg1 {
      int a, b;
   };

   DEF_SIMPLE_ASYNC_ACTION(Action1) {
      auto exec(const TransactionInfo&) -> TSL_NS::Status {
         return WAIN_ON(EV_MSG_1, handleMsg1);
      }

      DEF_INLINE_EVENT_HANDLER(handleMsg1, Msg1) {
         ASSERT_EQ(10, msg.a);
         ASSERT_EQ(20, msg.b);

         return Result::SUCCESS;
      }
   };


   FIXTURE(TestSimpleAsyncAction) {
      Action1 action1;
      Msg1 msg1{ 10, 20 };
      EV_NS::ConsecutiveEventInfo eventInfo{EV_MSG_1, msg1};
      Event event{eventInfo};
      StupidTransactionContext context;

      TEST("exec should return CONTINUE") {
         ASSERT_EQ(Result::CONTINUE, action1.exec(context));
      }

      TEST("if exec is called again, it should return OUT_OF_SCOPE") {
         ASSERT_EQ(Result::CONTINUE, action1.exec(context));
         ASSERT_EQ(Result::OUT_OF_SCOPE, action1.exec(context));
      }

      TEST("handleEvent should return SUCCESS") {
         ASSERT_EQ(Result::CONTINUE, action1.exec(context));
         ASSERT_EQ(Result::SUCCESS, action1.handleEvent(context, event));
      }

      TEST("if the EventId is not expected, handleEvent should return UNKNOWN_EVENT") {
         ASSERT_EQ(Result::CONTINUE, action1.exec(context));
         ASSERT_EQ(Result::UNKNOWN_EVENT, action1.handleEvent(context, Event(EV_NS::SimpleEventInfo(100))));
      }

      TEST("after event is accepted, event should be consumed") {
         ASSERT_EQ(Result::CONTINUE, action1.exec(context));

         ASSERT_FALSE(event.isConsumed());
         ASSERT_EQ(Result::SUCCESS, action1.handleEvent(context, event));
         ASSERT_TRUE(event.isConsumed());
      }

      TEST("after an action has been done successfully, if passing an event with same type, should return UNKNOWN_EVENT") {
         ASSERT_EQ(Result::CONTINUE, action1.exec(context));
         ASSERT_EQ(Result::SUCCESS, action1.handleEvent(context, event));
         ASSERT_EQ(Result::UNKNOWN_EVENT, action1.handleEvent(context, event));
      }

      TEST("when an action is waiting for an event, if it's killed, handleEvent will return UNKNOWN_EVENT") {
         ASSERT_EQ(Result::CONTINUE, action1.exec(context));
         action1.kill(context, Result::FAILED);
         ASSERT_EQ(Result::UNKNOWN_EVENT, action1.handleEvent(context, event));
      }
   };
}
