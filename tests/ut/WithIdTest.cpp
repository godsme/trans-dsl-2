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

   __def(Fork2, __params(__action(ACTION1), __action(ACTION2))) __as
   (__fork(1, __asyn(ACTION1)), __fork(2, __asyn(ACTION2)));


   TEST_CASE("__transaction with __with_id") {
      __def(Trans1) __as_trans
      ( __apply(Fork2, __with(AsyncAction1, AsyncAction4))
      , __asyn(AsyncAction2) // 24
      , __join()
      );

      GIVEN("a transaction without listener") {
         __def(Trans2) __as_trans
         ( __with_id(1, __apply(Fork2, __with(AsyncAction1, AsyncAction4)))
         , __with_id(2, __asyn(AsyncAction2))
         , __join()
         );

         THEN("its __with_id should be removed automatically") {
            REQUIRE(sizeof(Trans1) == sizeof(Trans2));
         }
      }

   }
}