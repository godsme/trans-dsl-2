//
// Created by Darwin Yuan on 2020/6/15.
//

#include <trans-dsl/sched/domain/Transaction.h>
#include <trans-dsl/utils/ActionStatus.h>

TSL_NS_BEGIN

auto Transaction::startWithEvent(Event const& event) -> Status {
   ActionStatus status = start();
   if(!status.isWorking()) {
      return status;
   }

   return handleEvent(event);
}

TSL_NS_END