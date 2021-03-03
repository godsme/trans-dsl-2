//
// Created by Darwin Yuan on 2020/6/15.
//

#include <trans-dsl/sched/action/SchedSafe.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/utils/AssertionHelper.h>

TSL_NS_BEGIN

///////////////////////////////////////////////////////////////////////////////
auto SchedSafe::exec(TransactionContext& context) -> Status {
   BUG_CHECK(state == State::IDLE);
   Status status = ROLE(SchedAction).exec(context);
   state = is_working_status(status) ? State::WORKING : State::DONE;
   return status;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSafe::handleEvent(TransactionContext& context, Event const& event) -> Status {
   BUG_CHECK(state == State::WORKING);
   Status status = ROLE(SchedAction).handleEvent(context, event);
   if(!is_working_status(status)) {
      state = State::DONE;
   }
   return status;
}
///////////////////////////////////////////////////////////////////////////////
auto SchedSafe::stop(TransactionContext&, Status) -> Status {
   BUG_CHECK(state == State::WORKING);
   return Result::CONTINUE;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSafe::kill(TransactionContext& context, Status cause) -> void {
   if(state == State::WORKING) {
      ROLE(SchedAction).kill(context, cause);
      state = State::DONE;
   }
}


TSL_NS_END