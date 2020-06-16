//
// Created by Darwin Yuan on 2020/6/16.
//

#include <trans-dsl/sched/action/SchedVoid.h>
#include <trans-dsl/utils/ActionStatus.h>

TSL_NS_BEGIN

///////////////////////////////////////////////////////////////////////////////
auto SchedVoid::exec(TransactionContext& context) -> Status {
   return FATAL_BUG;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedVoid::handleEvent(TransactionContext& context, const Event& event) -> Status {
   return FATAL_BUG;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedVoid::stop(TransactionContext& context, Status cause) -> Status {
   return FATAL_BUG;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedVoid::kill(TransactionContext& context, Status cause) -> void {
}


TSL_NS_END