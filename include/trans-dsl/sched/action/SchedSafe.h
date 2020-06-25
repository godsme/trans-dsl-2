//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_SCHEDSAFE_H
#define TRANS_DSL_2_SCHEDSAFE_H

#include <trans-dsl/sched/domain/SchedAction.h>
#include <trans-dsl/utils/ActionStatus.h>

TSL_NS_BEGIN

struct SchedSafe : SchedAction  {
   OVERRIDE(exec(TransactionContext& context) -> Status);
   OVERRIDE(handleEvent(TransactionContext&, Event const&) -> Status);
   OVERRIDE(kill(TransactionContext&, Status) -> void);
   OVERRIDE(stop(TransactionContext&, Status) -> Status);

private:
   enum State : uint8_t {
      IDLE,
      WORKING,
      DONE
   };

   State state = State::IDLE;

private:
   USE_ROLE(SchedAction);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDSAFE_H
