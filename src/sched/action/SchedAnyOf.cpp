//
// Created by Darwin Yuan on 2021/4/16.
//

#include <trans-dsl/sched/action/SchedAnyOf.h>
#include <trans-dsl/utils/AssertionHelper.h>

TSL_NS_BEGIN

namespace {
    inline auto stateStillWorking(SchedAnyOf::State state) -> bool {
        switch (state) {
            case SchedAnyOf::State::Working:
            case SchedAnyOf::State::Stopping:
                return true;
            default:
                return false;
        }
    }
}

inline auto SchedAnyOf::stillWorking() const -> bool {
    return stateStillWorking(state);
}

#define IS_CHILD_WORKING(i) stateStillWorking(children[i])

///////////////////////////////////////////////////////////////////////////////
auto SchedAnyOf::cleanUp(TransactionContext& context, Status status) -> void {
    auto cause = status == Result::SUCCESS ? Result::FORCE_STOPPED : status;
    for(SeqInt i=0; i < total; i++) {
        if(IS_CHILD_WORKING(i)) {
            get(i)->kill(context, cause);
        }
    }

    state = State::Done;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedAnyOf::exec(TransactionContext& context) -> Status {
    BUG_CHECK(state == State::Idle);

    total = getNumOfActions();

    for(SeqInt i=0; i < total; i++) {
        unlikely_branch
        if(Status status = get(i)->exec(context); unlikely(status != Result::CONTINUE)) {
            cleanUp(context, status);
            return status;
        }
        children[i] = State::Working;
    }

    state = State::Working;

    return Result::CONTINUE;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedAnyOf::handleEvent(TransactionContext& context, Event const& event) -> Status {
    BUG_CHECK(stillWorking());

    for(SeqInt i=0; i < total; i++) {
        if(!IS_CHILD_WORKING(i)) continue;
        Status status = get(i)->handleEvent(context, event);
        if(!(status & __WORKING_STATUS_BEGIN)) {
            children[i] = State::Done;
            cleanUp(context, status);
            return status;
        }

        if(event.isConsumed()) {
            return Result::CONTINUE;
        }
    }

    return Result::UNKNOWN_EVENT;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedAnyOf::stopAll(TransactionContext& context, Status cause) -> Status {
    for(SeqInt i=0; i < total; i++) {
        if(!IS_CHILD_WORKING(i)) continue;
        Status status = get(i)->stop(context, cause);
        if(!(status & __WORKING_STATUS_BEGIN)) {
            children[i] = State::Done;
            cleanUp(context, status);
            return status;
        }
    }

    state = State::Stopping;

    return Result::CONTINUE;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedAnyOf::stop(TransactionContext& context, Status cause) -> Status {
    switch (state) {
        likely_branch
        case State::Working: {
            return stopAll(context, cause);
        }
        case State::Stopping:
            return Result::CONTINUE;
        unlikely_branch
        default:
            return Result::FATAL_BUG;
    }
}

///////////////////////////////////////////////////////////////////////////////
auto SchedAnyOf::kill(TransactionContext& context, Status cause) -> void {
    likely_branch
    if(likely(stillWorking())) {
        cleanUp(context, cause);
    }
}

TSL_NS_END
