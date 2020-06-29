//
// Created by Darwin Yuan on 2020/6/16.
//

#include <trans-dsl/sched/action/SchedWait.h>
#include <trans-dsl/utils/ActionStatus.h>

TSL_NS_BEGIN

///////////////////////////////////////////////////////////////////////////////
auto SchedWait::exec(TransactionContext&) -> Status {
   return Result::CONTINUE;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedWait::handleEvent(TransactionContext&, Event const& event) -> Status {
   if(event.matches(getEventId()))
   {
      if(isWait()) event.consume();
      return Result::SUCCESS;
   }

   return Result::UNKNOWN_EVENT;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedWait::stop(TransactionContext&, Status) -> Status {
   return Result::FORCE_STOPPED;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedWait::kill(TransactionContext&, Status) -> void {
}


TSL_NS_END