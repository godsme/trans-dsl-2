//
// Created by Darwin Yuan on 2020/6/16.
//

#include <cctest/cctest.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <trans-dsl/sched/helper/FragmentHelper.h>
#include <trans-dsl/trans-dsl.h>

namespace {
   using namespace TSL_NS;

   __def(Fragment1) = __async(AsyncAction1);


   FIXTURE(TestOptional) {
      Fragment1 action1;

      StupidTransactionContext context{};

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      TEST("should be able exec fragment") {
         ASSERT_EQ(Result::CONTINUE, action1.exec(context));
         ASSERT_EQ(Result::SUCCESS, action1.handleEvent(context, event1));
      }
   };

   __def(Fragment2, __params(__action(T1), __action(T2))) = __sequential(T1, T2);

   FIXTURE(TestOptional1) {
      __apply(Fragment2, __with(__async(AsyncAction1), __async(AsyncAction2))) fragment;

      StupidTransactionContext context{};

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo eventInfo1{EV_MSG_1, msg1};
      TSL_NS::Event event1{eventInfo1};

      const Msg2 msg2{ 30 };
      const EV_NS::ConsecutiveEventInfo eventInfo2{EV_MSG_2, msg2};
      TSL_NS::Event event2{eventInfo2};

      TEST("should be able exec fragment") {
         ASSERT_EQ(Result::CONTINUE, fragment.exec(context));
         ASSERT_EQ(Result::CONTINUE, fragment.handleEvent(context, event1));
         ASSERT_EQ(Result::SUCCESS, fragment.handleEvent(context, event2));
      }
   };

}