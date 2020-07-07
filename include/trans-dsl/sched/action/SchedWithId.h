//
// Created by Darwin Yuan on 2020/7/7.
//

#ifndef TRANS_DSL_2_SCHEDWITHID_H
#define TRANS_DSL_2_SCHEDWITHID_H

#include <trans-dsl/sched/domain/SchedAction.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/domain/ActionId.h>

TSL_NS_BEGIN

struct SchedWithId : SchedAction  {
   OVERRIDE(exec(TransactionContext& context) -> Status);
   OVERRIDE(handleEvent(TransactionContext&, Event const&) -> Status);
   OVERRIDE(kill(TransactionContext&, Status) -> void);
   OVERRIDE(stop(TransactionContext&, Status) -> Status);

private:
   enum class State {
      INIT,
      WORKING,
      STOPPING,
      DONE
   };

   State state = State::INIT;
   ActionId actionId = 0;

private:
   ABSTRACT(getActionId() const -> ActionId);
   USE_ROLE(SchedAction);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDWITHID_H
