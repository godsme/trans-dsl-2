//
// Created by Darwin Yuan on 2020/7/7.
//
#include <catch.hpp>
#include <trans-dsl/trans-dsl.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <iostream>
#include <trans-dsl/sched/helper/TransactionListenerHelper.h>
#include <trans-dsl/sched/domain/ObservedActionIdRegistry.h>


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

   struct C : ObservedActionIdRegistry<> {
      auto onActionStarting(ActionId aid, TransactionInfo const&) -> void {
         switch(aid) {
            case 1: C_1_reported = true; break;
            case 2: C_2_reported = true; break;
            case 3: C_3_reported = true; break;
            default: unexpected_aid = true; break;
         }
      }
   };


   TEST_CASE("ObservedActionIdRegistry") {
      REQUIRE(ObservedActionIdRegistry<1, 2>::Aids.isEnabled(1));
      REQUIRE(ObservedActionIdRegistry<1, 2>::Aids.isEnabled(2));
      REQUIRE(!ObservedActionIdRegistry<1, 2>::Aids.isEnabled(3));
   }

   TEST_CASE("TransactionListenerDispatcher") {
      TSL_NS::details::TransactionListenerDispatcher<A, B, C> dispatcher;
      clear_all_flags();

      StupidTransactionContext context{};

      WHEN("report a AID=1 onActionStarting event") {
         dispatcher.onActionStarting(1, context);
         THEN("should have A&C get reported") {
            REQUIRE(A_1_reported);
            REQUIRE(C_1_reported);

            REQUIRE(!A_2_reported);
            REQUIRE(!B_2_reported);
            REQUIRE(!B_3_reported);
            REQUIRE(!C_2_reported);
            REQUIRE(!C_3_reported);
            REQUIRE(!unexpected_aid);
         }
      }
      WHEN("report a AID=2 onActionStarting event") {
         dispatcher.onActionStarting(2, context);
         THEN("should have A, B, C all got reported") {
            REQUIRE(A_2_reported);
            REQUIRE(B_2_reported);
            REQUIRE(C_2_reported);

            REQUIRE(!A_1_reported);
            REQUIRE(!B_3_reported);
            REQUIRE(!C_1_reported);
            REQUIRE(!C_3_reported);
            REQUIRE(!unexpected_aid);
         }
      }

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      WHEN("report a AID=2 onActionEventConsumed event") {
         dispatcher.onActionEventConsumed(2, context, event1);
         THEN("should have A both got reported") {
            REQUIRE(A_2_reported);

            REQUIRE(!B_2_reported);
            REQUIRE(!A_1_reported);
            REQUIRE(!B_3_reported);
            REQUIRE(!C_1_reported);
            REQUIRE(!C_2_reported);
            REQUIRE(!C_3_reported);
            REQUIRE(!unexpected_aid);
         }
      }
   }
}
