//
// Created by Darwin Yuan on 2021/3/22.
//

#ifndef TRANS_DSL_2_SCHEDEXCLUSIVE_H
#define TRANS_DSL_2_SCHEDEXCLUSIVE_H

#include <trans-dsl/sched/domain/SchedAction.h>
#include <trans-dsl/sched/domain/RuntimeContext.h>
#include <trans-dsl/utils/SeqInt.h>

TSL_NS_BEGIN

struct SchedExclusive
        : SchedAction  {

    OVERRIDE(exec(TransactionContext&)                      -> Status);
    OVERRIDE(handleEvent(TransactionContext&, Event const&) -> Status);
    OVERRIDE(stop(TransactionContext&, Status cause)        -> Status);
    OVERRIDE(kill(TransactionContext&, Status cause)        -> void);

private:
    auto cleanUp(TransactionContext &context, SeqInt last, Status failStatus) -> Status;
    auto startUp(TransactionContext&) -> Status;
    auto cleanUp(TransactionContext& context, Status failStatus) -> Status;
    auto cleanUpExcept(TransactionContext &context, SeqInt last, Status cause) -> void;
    auto handleEvent_(TransactionContext&, Event const&) -> Status;
    auto handleEventOnSelecting(TransactionContext& context, Event const& event) -> Status;
    auto handleEventOnSelected(TransactionContext & context, const Event & event) -> Status;
    auto stopOnSelected(TransactionContext & context, Status cause) -> Status;

private:
    enum class State : uint8_t {
        Idle,
        Selecting,
        Working,
        Stopping,
        Done
    };

protected:
    enum {
        Max_Num_Of_Children = 6
    };

private:
    SchedAction* selected{};
    SeqInt total = 0;
    State state = State::Idle;

private:
    ABSTRACT(getNumOfActions() const -> SeqInt);
    ABSTRACT(get(SeqInt index) -> SchedAction*);
};

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDEXCLUSIVE_H
