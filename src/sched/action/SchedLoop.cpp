//
// Created by Darwin Yuan on 2020/6/12.
//

#include <trans-dsl/sched/action/SchedLoop.h>

TSL_NS_BEGIN

auto SchedLoop::exec(TransactionContext& ) -> Status {
   return Result::FATAL_BUG;
}

auto SchedLoop::handleEvent(TransactionContext&, const Event&) -> Status {
   return Result::FATAL_BUG;
}

auto SchedLoop::stop(TransactionContext& ) -> Status {
   return Result::FATAL_BUG;
}

auto SchedLoop::kill(TransactionContext& ) -> void {
}

TSL_NS_END
