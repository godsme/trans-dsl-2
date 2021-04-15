//
// Created by Darwin Yuan on 2021/4/16.
//

#ifndef TRANS_DSL_2_SCHEDANYOF_H
#define TRANS_DSL_2_SCHEDANYOF_H

#include <trans-dsl/sched/domain/SchedAction.h>
#include <trans-dsl/sched/domain/RuntimeContext.h>
#include <trans-dsl/utils/SeqInt.h>

TSL_NS_BEGIN

struct SchedAnyOf : SchedAction  {
    OVERRIDE(exec(TransactionContext&)                      -> Status);
    OVERRIDE(handleEvent(TransactionContext&, Event const&) -> Status);
    OVERRIDE(stop(TransactionContext&, Status cause)        -> Status);
    OVERRIDE(kill(TransactionContext&, Status cause)        -> void);

private:
    auto cleanUp(TransactionContext&, Status cause) -> void;
    auto stopAll(TransactionContext&, Status cause) -> Status;
    auto stillWorking() const -> bool;

public:
    enum class State : uint8_t {
        Idle,
        Working,
        Stopping,
        Done
    };

private:
    enum {
        Max_Num_Of_Children = 6
    };

private:
    SeqInt total = 0;
    State state{State::Idle};
    State children[Max_Num_Of_Children]{};

private:
    ABSTRACT(getNumOfActions() const -> SeqInt);
    ABSTRACT(get(SeqInt index) -> SchedAction*);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDANYOF_H
