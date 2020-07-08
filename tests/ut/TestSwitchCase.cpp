//
// Created by Darwin Yuan on 2020/6/15.
//

#include <catch.hpp>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/trans-dsl.h>

namespace {
   using namespace TSL_NS;

   struct A : ObservedActionIdRegistry<1, 2> {
      auto onActionStarting(ActionId, TransactionInfo const&) -> void {}
      auto onActionEventConsumed(ActionId, TransactionInfo const&, Event const&) -> void {}
   };

   struct B : ObservedActionIdRegistry<2, 3> {
      auto onActionStarting(ActionId, TransactionInfo const&) -> void {}
   };

   struct C : ObservedActionIdRegistry<1, 2, 3> {
      auto onActionStarting(ActionId, TransactionInfo const&) -> void {}
   };

   struct D : ObservedActionIdRegistry<> {
      auto onActionStarting(ActionId, TransactionInfo const&) -> void {}
   };

   using Listeners = __listeners(A, B, C);

   SCENARIO("__switch with __with_id", "[switch]") {
      __def(Trans) __as_trans
      ( __switch(
          __case(__is_status(Result::OUT_OF_SCOPE), __asyn(AsyncAction1)),
          __case(__is_status(Result::INVALID_DATA), __asyn(AsyncAction2)),
          __otherwise(__asyn(AsyncAction4))));

      __def(TransWithId) __as_trans
      ( __switch(
          __case(__is_status(Result::OUT_OF_SCOPE), __with_id(1, __asyn(AsyncAction1))),
          __case(__is_status(Result::INVALID_DATA), __with_id(2, __asyn(AsyncAction2))),
          __otherwise(__with_id(3, __asyn(AsyncAction4)))));

      REQUIRE(sizeof(Trans) == sizeof(TransWithId));

      WHEN("bind a group listeners, __with_id should not be removed") {
         using Trans2WithListener = __bind_listener(TransWithId, Listeners);

         Trans2WithListener trans;
         REQUIRE(sizeof(Trans2WithListener) == sizeof(Trans) + sizeof(SchedWithId) + 8);
      }
   }

   SCENARIO("__switch test", "[switch]") {
      StupidTransactionContext context{};

      __def_switch(
         __case(__is_status(Result::OUT_OF_SCOPE), __asyn(AsyncAction1)),
         __case(__is_status(Result::INVALID_DATA), __asyn(AsyncAction2)),
         __otherwise(__asyn(AsyncAction4))) action;

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      const Msg4 msg4{ 30 };
      const EV_NS::ConsecutiveEventInfo event4{EV_MSG_4, msg4};

      GIVEN("the runtime env has an OUT_OF_SCOPE error") {
         context.getRuntimeContext().reportFailure(Result::OUT_OF_SCOPE);
         REQUIRE(Result::CONTINUE == action.exec(context));

         THEN("should choose path 1") {
            REQUIRE(Result::SUCCESS  == action.handleEvent(context, event1));
         }

         WHEN("event2 received, should return UNKNOWN_EVENT") {
            REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event2));
         }

         WHEN("event 1 received, should return UNKNOWN_EVENT") {
            REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event4));
         }
      }

      GIVEN("the runtime env has an INVALID_DATA error") {
         context.getRuntimeContext().reportFailure(Result::INVALID_DATA);
         REQUIRE(Result::CONTINUE == action.exec(context));

         THEN("should choose path 2") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
         }

         WHEN("event 1 received, should return UNKNOWN_EVENT") {
            REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event1));
         }

         WHEN("event 4 received, should return UNKNOWN_EVENT") {
            REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event4));
         }
      }

      GIVEN("the runtime env has a TIMEOUT error") {
         context.getRuntimeContext().reportFailure(Result::TIMEOUT);
         REQUIRE(Result::CONTINUE == action.exec(context));

         THEN("should choose default path") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
         }

         WHEN("event 1 received,should return UNKNOWN_EVENT") {
            REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event1));

            AND_WHEN("event 4 received, should return SUCCESS") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
            }

            AND_WHEN("event 2 received, should return UNKNOWN_EVENT") {
               REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event2));

               AND_WHEN("event 4 received, should return SUCCESS") {
                  REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
               }
            }
         }

         WHEN("event 2 received, should return UNKNOWN_EVENT") {
            REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event2));

            AND_WHEN("event 4 received, should return SUCCESS") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
            }
         }
      }
   };
}