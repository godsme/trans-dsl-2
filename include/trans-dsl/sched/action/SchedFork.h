//
// Created by Darwin Yuan on 2020/7/1.
//

#ifndef TRANS_DSL_2_SCHEDFORK_H
#define TRANS_DSL_2_SCHEDFORK_H

#include <trans-dsl/tsl_status.h>
#include <cub/dci/Role.h>
#include <trans-dsl/sched/action/SchedSyncAction.h>

TSL_NS_BEGIN

struct TransactionContext;

struct SchedFork : SchedSyncAction {
   OVERRIDE(exec(TransactionContext&) -> Status);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDFORK_H
