//
// Created by Darwin Yuan on 2020/6/16.
//

#include <trans-dsl/sched/action/SchedVoid.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/concept/RuntimeContextAutoSwitch.h>

TSL_NS_BEGIN

///////////////////////////////////////////////////////////////////////////////
namespace {
   struct AutoSwitch
      : private RuntimeContext
      , RuntimeContextAutoSwitch {
      AutoSwitch(TransactionContext& transactionContext)
         : RuntimeContextAutoSwitch(transactionContext, *this) {}
   };
}

///////////////////////////////////////////////////////////////////////////////
#define AUTO_SWITCH() AutoSwitch autoSwitch__{context}

///////////////////////////////////////////////////////////////////////////////
#define __CALL(call) do {                                \
   AUTO_SWITCH();                                        \
   ActionStatus status = ROLE(SchedAction).call;         \
   return status.isWorking() ? status : Result::SUCCESS; \
} while(0)

///////////////////////////////////////////////////////////////////////////////
auto SchedVoid::exec(TransactionContext& context) -> Status {
   __CALL(exec(context));
}

///////////////////////////////////////////////////////////////////////////////
auto SchedVoid::handleEvent(TransactionContext& context, const Event& event) -> Status {
   __CALL(handleEvent(context, event));
}

///////////////////////////////////////////////////////////////////////////////
auto SchedVoid::stop(TransactionContext& context, Status cause) -> Status {
   __CALL(stop(context, cause));
}

///////////////////////////////////////////////////////////////////////////////
auto SchedVoid::kill(TransactionContext& context, Status cause) -> void {
   AUTO_SWITCH();
   return ROLE(SchedAction).kill(context, cause);
}


TSL_NS_END