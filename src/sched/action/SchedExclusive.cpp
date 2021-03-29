//
// Created by Darwin Yuan on 2021/3/22.
//

#include <trans-dsl/sched/action/SchedExclusive.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/tsl_config.h>

TSL_NS_BEGIN

auto SchedExclusive::cleanUp(TransactionContext& context, Status cause) -> Status {
    for(SeqInt i=0; i < total; i++) {
        get(i)->kill(context, cause);
    }

    state = State::Done;

    return cause;
}

auto SchedExclusive::cleanUp(TransactionContext &context, SeqInt last, Status cause) -> Status {
    for(SeqInt i=0; i < last; i++) {
        get(i)->kill(context, cause);
    }

    state = State::Done;

    return cause;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedExclusive::cleanUpExcept(TransactionContext &context, SeqInt except, Status cause) -> void {
    for(SeqInt i=0; i < total; i++) {
        if(i != except) {
            get(i)->kill(context, cause);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
auto SchedExclusive::startUp(TransactionContext& context) -> Status {
    for(SeqInt i=0; i < total; i++) {
        auto action = get(i);

        unlikely_branch
        if(unlikely(action == nullptr)) {
            return cleanUp(context, i, Result::FATAL_BUG);
        }

        Status status = action->exec(context);
        likely_branch
        if(likely(status != Result::CONTINUE)) {
            return cleanUp(context, i, status);
        }
    }

    state = State::Selecting;

    return Result::CONTINUE;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedExclusive::exec(TransactionContext& context) -> Status {
    unlikely_branch
    if(unlikely(state != State::Idle)) {
        return FATAL_BUG;
    }

    total = getNumOfActions();

    return startUp(context);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedExclusive::handleEventOnSelecting(TransactionContext& context, Event const& event) -> Status {
    for(SeqInt i=0; i<total; i++) {
        auto action = get(i);
        auto status = action->handleEvent(context, event);
        if(status != Result::UNKNOWN_EVENT) {
            cleanUpExcept(context, i, status);
            if(status == Result::CONTINUE) {
                selected = action;
                state = State::Working;
            } else {
                state = State::Done;
            }

            return status;
        }
    }

    return Result::UNKNOWN_EVENT;
}

auto SchedExclusive::handleEventOnSelected(TransactionContext & context, const Event & event) -> Status {
    auto status = selected->handleEvent(context, event);
    if(status != Result::CONTINUE && status != Result::UNKNOWN_EVENT) {
        state = State::Done;
    }
    return status;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedExclusive::handleEvent(TransactionContext& context, const Event& event) -> Status {
    switch (state) {
        case State::Selecting:
            return handleEventOnSelecting(context, event);
        case State::Working:
        case State::Stopping:
            return handleEventOnSelected(context, event);
        default:
            return Result::FATAL_BUG;
    }
}


auto SchedExclusive::stopOnSelected(TransactionContext & context, Status cause) -> Status {
    auto status = selected->stop(context, cause);
    if(status != Result::CONTINUE && status != Result::UNKNOWN_EVENT) {
        state = State::Done;
    }
    return status;
}
///////////////////////////////////////////////////////////////////////////////
auto SchedExclusive::stop(TransactionContext& context, Status cause) -> Status {
    switch (state) {
        likely_branch
        case State::Selecting: {
            (void)cleanUp(context, cause);
            return cause;
        }
        case State::Working: {
            return stopOnSelected(context, cause);
        }
        case State::Stopping:
            return Result::CONTINUE;
            unlikely_branch
        default:
            return Result::FATAL_BUG;
    }
}

///////////////////////////////////////////////////////////////////////////////
auto SchedExclusive::kill(TransactionContext& context, Status cause) -> void {
    switch (state) {
        likely_branch
        case State::Selecting: {
            cleanUp(context, cause);
            break;
        }
        case State::Working:
        case State::Stopping: {
            selected->kill(context, cause);
            break;
        }
        default: break;
    }

    state = State::Done;
}

TSL_NS_END