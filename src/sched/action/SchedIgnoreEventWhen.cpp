//
// Created by Darwin Yuan on 2021/4/26.
//

#include <trans-dsl/sched/action/SchedIgnoreEventWhen.h>

TSL_NS_BEGIN

///////////////////////////////////////////////////////////////////////////////
auto SchedIgnoreEventWhen::exec(TransactionContext& context) -> Status {
    return ROLE(SchedAction).exec(context);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedIgnoreEventWhen::handleEvent(TransactionContext& context, Event const& event) -> Status {
    auto status = ROLE(SchedAction).handleEvent(context, event);
    if(status != Result::UNKNOWN_EVENT) {
        return status;
    }

    if(!matches(event.getEventId())) {
        return Result::UNKNOWN_EVENT;
    }

    event.consume();
    return Result::CONTINUE;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedIgnoreEventWhen::stop(TransactionContext& context, Status cause) -> Status {
    return ROLE(SchedAction).stop(context, cause);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedIgnoreEventWhen::kill(TransactionContext& context, Status cause) -> void {
    ROLE(SchedAction).kill(context, cause);
}

TSL_NS_END
