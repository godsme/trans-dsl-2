//
// Created by Darwin Yuan on 2020/7/1.
//

#ifndef TRANS_DSL_2_SCHEDJOIN_H
#define TRANS_DSL_2_SCHEDJOIN_H

#include <trans-dsl/sched/domain/SchedAction.h>
#include <trans-dsl/sched/domain/ThreadBitMap.h>

TSL_NS_BEGIN

struct TransactionContext;

struct SchedJoin : SchedAction {
   OVERRIDE(exec(TransactionContext&)                      -> Status);
   OVERRIDE(handleEvent(TransactionContext&, Event const&) -> Status);
   OVERRIDE(stop(TransactionContext&, Status)              -> Status);
   OVERRIDE(kill(TransactionContext&, Status)              -> void);

private:
   ThreadBitMap bitMap{};

private:
   ABSTRACT(getThreadBitMap() const -> ThreadBitMap);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDJOIN_H
