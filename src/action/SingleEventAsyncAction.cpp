//
// Created by godsme on 2021/3/5.
//

#include <trans-dsl/action/SingleEventAsyncAction.h>

TSL_NS_BEGIN

auto SingleEventAsyncAction::handleEvent(TransactionInfo const& trans, Event const& event) -> Status {
    if(handler.index() == 0) {
        return USER_FATAL_BUG;
    }

    if(!(eventId == event.getEventId() && matchesMore(event))) {
        return Result::UNKNOWN_EVENT;
    }

    event.consume();
    Status status = FATAL_BUG;
    switch (handler.index()) {
        case 1: {
            status = std::get<1>(handler)(nullptr, trans, *reinterpret_cast<details::DummyMsgType const*>(event.getMsg()));
            break;
        }
        case 2: {
            status = std::get<2>(handler)(trans, *reinterpret_cast<details::DummyMsgType const*>(event.getMsg()));
            break;
        }
        default: /* not possible */ return FATAL_BUG;
    }

    reset();

    return status;
}

auto SingleEventAsyncAction::matchesMore(Event const&) const -> bool {
    return true;
}

auto SingleEventAsyncAction::kill(TransactionInfo const&, Status) -> Status {
    if(handler.index() != 0) {
        reset();
        return Result::SUCCESS;
    }
    return Result::USER_FATAL_BUG;
}

auto SingleEventAsyncAction::reset() -> void {
    this->handler = std::monostate{};
    this->eventId = INVALID_EVENT_ID;
}

template<typename HANDLER_TYPE>
auto SingleEventAsyncAction::doAddHandler(EventId eventId, HANDLER_TYPE handler) -> Status {
    if(this->handler.index() != 0) {
        return Result::OUT_OF_SCOPE;
    }

    this->handler = handler;
    this->eventId = eventId;

    return Result::CONTINUE;
}

auto SingleEventAsyncAction::addHandler(EventId eventId, details::NormalFunction handler) -> Status {
    return doAddHandler(eventId, handler);
}

auto SingleEventAsyncAction::addHandler(EventId eventId, details::MemberFunction handler) -> Status {
    return doAddHandler(eventId, handler);
}

TSL_NS_END