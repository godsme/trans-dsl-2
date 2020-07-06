//
// Created by Darwin Yuan on 2020/7/6.
//

#include <catch.hpp>
#include <trans-dsl/trans-dsl.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <iostream>

namespace {
   using namespace TSL_NS;

   __def(Fork2, __params(__action(ACTION1), __action(ACTION2))) __as
   ( __fork(1, __asyn(ACTION1))
   , __fork(2, __asyn(ACTION2)));


   SCENARIO("__transaction with a segment") {

      using Trans =
      __transaction
      ( __apply(Fork2, __with(AsyncAction1, AsyncAction4)) //32
      , __asyn(AsyncAction2) // 24
      , __join()
      );

      using Trans2 =
      __transaction
      ( __fork(1, __asyn(AsyncAction1))
      , __fork(2, __asyn(AsyncAction4))
      , __asyn(AsyncAction2) // 24
      , __join()
      );

      using Trans3 =
      __transaction
      ( __sequential
         ( __fork(1, __asyn(AsyncAction1))
         , __fork(2, __asyn(AsyncAction4))
         , __asyn(AsyncAction2)
         , __join())
      );

      using Trans4 =
      __transaction
      ( __sequential(
         __sequential
           ( __fork(1, __asyn(AsyncAction1))
           , __fork(2, __asyn(AsyncAction4)))
         , __asyn(AsyncAction2))
         , __join()
      );

      // inline-seq
      REQUIRE(sizeof(Trans) == sizeof(Trans2));
      REQUIRE(sizeof(Trans3) == sizeof(Trans2));
      REQUIRE(sizeof(Trans4) == sizeof(Trans2));

      Trans trans;

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{10};
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      const Msg4 msg4{10};
      const EV_NS::ConsecutiveEventInfo event4{EV_MSG_4, msg4};

      WHEN("start, should return CONTINUE") {
         REQUIRE(Result::CONTINUE == trans.start());
         WHEN("event 2 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == trans.handleEvent(event2));
            THEN("event 4 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == trans.handleEvent(event4));
               THEN("event 1 received, should return SUCCESS") {
                  REQUIRE(Result::SUCCESS == trans.handleEvent(event1));
               }
            }
         }
      }

      WHEN("exec with a sync action, should return CONTINUE") {
         REQUIRE(Result::CONTINUE == trans.start());
         WHEN("event 1 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == trans.handleEvent(event1));
            THEN("event 4 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == trans.handleEvent(event4));
               THEN("event 2 received, should return SUCCESS") {
                  REQUIRE(Result::SUCCESS == trans.handleEvent(event2));
               }
            }
         }
      }
   }

   __def(Fragment1) __as(__asyn(AsyncAction1));

   SCENARIO("Simple fragment") {
      Fragment1 action1;

      StupidTransactionContext context{};

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      WHEN("should be able exec fragment") {
         REQUIRE(Result::CONTINUE == action1.exec(context));
         REQUIRE(Result::SUCCESS == action1.handleEvent(context, event1));
      }
   };

   __def(Fragment2, __params(__action(T1), __action(T2))) __as (
      __sequential(T1, T2)
   );

   SCENARIO("fragment with parameters") {
      __apply(Fragment2, __with(__asyn(AsyncAction1), __asyn(AsyncAction2))) fragment;

      StupidTransactionContext context{};

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      WHEN("should be able exec fragment") {
         REQUIRE(Result::CONTINUE == fragment.exec(context));
         REQUIRE(Result::CONTINUE == fragment.handleEvent(context, event1));
         REQUIRE(Result::SUCCESS  == fragment.handleEvent(context, event2));
      }
   };

}