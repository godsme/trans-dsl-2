//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_SCHEDACTION_H
#define TRANS_DSL_2_SCHEDACTION_H

#include <trans-dsl/sched/concept/FinalAction.h>

TSL_NS_BEGIN

struct SchedAction : FinalAction {
   ABSTRACT(stop(TransactionContext&, Status) -> Status);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDACTION_H
