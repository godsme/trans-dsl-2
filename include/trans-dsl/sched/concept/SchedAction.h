//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_SCHEDACTION_H
#define TRANS_DSL_2_SCHEDACTION_H

#include <cub/dci/Role.h>
#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/concept/Event.h>

TSL_NS_BEGIN

struct TransactionContext;

DEFINE_ROLE(SchedAction)  {
   ABSTRACT(exec(TransactionContext&)                      -> Status);
   ABSTRACT(handleEvent(TransactionContext&, const Event&) -> Status);
   ABSTRACT(stop(TransactionContext&, Status cause)        -> Status);
   ABSTRACT(kill(TransactionContext&, Status cause)        -> void);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDACTION_H
