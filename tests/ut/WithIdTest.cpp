//
// Created by Darwin Yuan on 2020/7/7.
//

#include <catch.hpp>
#include <trans-dsl/trans-dsl.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <iostream>

namespace {
   using namespace TSL_NS;

   bool A_1_reported = false;
   bool A_2_reported = false;
   bool B_2_reported = false;
   bool B_3_reported = false;
   bool C_1_reported = false;
   bool C_2_reported = false;
   bool C_3_reported = false;
   bool unexpected_aid = false;

   void clear_all_flags() {
      A_1_reported = false;
      A_2_reported = false;
      B_2_reported = false;
      B_3_reported = false;
      C_1_reported = false;
      C_2_reported = false;
      C_3_reported = false;
      unexpected_aid = false;
   }

   struct A : ObservedActionIdRegistry<1, 2> {
      auto onActionStarting(ActionId aid, TransactionInfo const&) -> void {
         switch(aid) {
            case 1: A_1_reported = true; break;
            case 2: A_2_reported = true; break;
            default: unexpected_aid = true; break;
         }
      }

      auto onActionEventConsumed(ActionId aid, TransactionInfo const&, Event const&) -> void {
         switch(aid) {
            case 1: A_1_reported = true; break;
            case 2: A_2_reported = true; break;
            default: unexpected_aid = true; break;
         }
      }
   };

   struct B : ObservedActionIdRegistry<2, 3> {
      auto onActionStarting(ActionId aid, TransactionInfo const&) -> void {
         switch(aid) {
            case 2: B_2_reported = true; break;
            case 3: B_3_reported = true; break;
            default: unexpected_aid = true; break;
         }
      }
   };

   struct C : ObservedActionIdRegistry<1, 2, 3> {
      auto onActionStarting(ActionId aid, TransactionInfo const&) -> void {
         switch(aid) {
            case 1: C_1_reported = true; break;
            case 2: C_2_reported = true; break;
            case 3: C_3_reported = true; break;
            default: unexpected_aid = true; break;
         }
      }
   };

   struct D : ObservedActionIdRegistry<> {
      auto onActionStarting(ActionId aid, TransactionInfo const&) -> void {
         switch(aid) {
            case 1: C_1_reported = true; break;
            case 2: C_2_reported = true; break;
            case 3: C_3_reported = true; break;
            default: unexpected_aid = true; break;
         }
      }
   };

   __def(Fork2, __params(__action(ACTION1), __action(ACTION2))) __as
   (__fork(1, __with_id(1, __asyn(ACTION1))), __fork(2, __with_id(2, __asyn(ACTION2))));

   using Listeners = __listeners(A, B, C);

   TEST_CASE("__transaction with __with_id") {
      __def(Trans1) __as_trans
      ( __apply(Fork2, __with(AsyncAction1, AsyncAction4))
      , __asyn(AsyncAction2) // 24
      , __join()
      );

      GIVEN("a transaction has multi-with-id actions") {
         __def(Trans2) __as_trans
         ( __with_id(4, __apply(Fork2, __with(AsyncAction1, AsyncAction4)))
         , __with_id(3, __asyn(AsyncAction2))
         , __join()
         );

         clear_all_flags();
         THEN("its __with_id should be removed automatically") {
            REQUIRE(sizeof(Trans1) == sizeof(Trans2));
         }

         WHEN("bind a group listeners, __with_id should not be removed") {
            using Trans2WithListener = __bind_listener(Trans2, Listeners);

            Trans2WithListener trans;
            REQUIRE(sizeof(Trans2WithListener) > sizeof(Trans2));
            REQUIRE(Result::CONTINUE == trans.start());
         }
      }
   }
}