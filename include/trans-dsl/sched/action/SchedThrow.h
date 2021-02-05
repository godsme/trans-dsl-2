//
// Created by godsme on 2021/2/5.
//

#ifndef TRANS_DSL_2_SCHEDTHROW_H
#define TRANS_DSL_2_SCHEDTHROW_H

#include <trans-dsl/sched/action/SchedSyncAction.h>

TSL_NS_BEGIN

struct SchedThrow : public SchedSyncAction {
    OVERRIDE(exec(TransactionContext&) -> Status);

private:
    virtual auto getStatus() const -> Status = 0;
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDTHROW_H
