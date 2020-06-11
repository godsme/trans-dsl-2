//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_SCHEDOPTIONAL_H
#define TRANS_DSL_2_SCHEDOPTIONAL_H

#include <trans-dsl/tsl_status.h>
#include <cub/dci/Role.h>
#include <trans-dsl/sched/concept/SchedAction.h>

TSL_NS_BEGIN

struct TransactionContext;

struct SchedOptional : SchedAction {

   OVERRIDE(exec(TransactionContext&)                      -> Status);
   OVERRIDE(handleEvent(TransactionContext&, const Event&) -> Status);
   OVERRIDE(stop(TransactionContext&)                      -> Status);
   OVERRIDE(kill(TransactionContext&)                      -> void);

private:
   SchedAction* action = nullptr;

private:
   ABSTRACT(getAction() -> SchedAction*);
   ABSTRACT(isTrue(TransactionContext&) const -> bool);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDOPTIONAL_H
