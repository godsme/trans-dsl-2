//
// Created by Darwin Yuan on 2020/7/1.
//

#ifndef TRANS_DSL_2_MULTITHREADCONTEXT_H
#define TRANS_DSL_2_MULTITHREADCONTEXT_H

#include <cub/dci/Role.h>
#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/domain/ThreadBitMap.h>
#include <trans-dsl/sched/domain/ThreadId.h>

TSL_NS_BEGIN

struct SchedAction;
struct TransactionContext;

DEFINE_ROLE(MultiThreadContext) {
   ABSTRACT(join(ThreadBitMap&) -> Status);
   ABSTRACT(startThread(TransactionContext&, ThreadId) -> Status);
};

TSL_NS_END

#endif //TRANS_DSL_2_MULTITHREADCONTEXT_H
