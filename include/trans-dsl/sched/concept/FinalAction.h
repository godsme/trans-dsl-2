//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_FINALACTION_H
#define TRANS_DSL_2_FINALACTION_H

#include <trans-dsl/tsl_ns.h>
#include <cub/dci/Role.h>
#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/concept/Event.h>

TSL_NS_BEGIN

struct TransactionContext;

DEFINE_ROLE(FinalAction) {
   ABSTRACT(exec(TransactionContext&)                -> Status);
   ABSTRACT(handleEvent(TransactionContext&, Event&) -> Status);
   ABSTRACT(kill(TransactionContext&, Status)        -> void);
};

TSL_NS_END

#endif //TRANS_DSL_2_FINALACTION_H
