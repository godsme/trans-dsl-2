//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_SCHEDSWITCHCASE_H
#define TRANS_DSL_2_SCHEDSWITCHCASE_H

#include <trans-dsl/sched/domain/SchedAction.h>

TSL_NS_BEGIN

struct ActionPath;

// 16 bytes
struct SchedSwitchCase : SchedAction {
   OVERRIDE(exec(TransactionContext&) -> Status);
   OVERRIDE(handleEvent(TransactionContext&, Event const&) -> Status);
   OVERRIDE(stop(TransactionContext&, Status) -> Status);
   OVERRIDE(kill(TransactionContext&, Status) -> void);

private:
   SchedAction* action = nullptr;

private:
   ABSTRACT(getNext() -> ActionPath *);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDSWITCHCASE_H
