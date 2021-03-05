//
// Created by godsme on 2021/3/5.
//

#include <trans-dsl/action/SequencedAsyncAction.h>

TSL_NS_BEGIN

auto SequencedAsyncAction::matches(Event const& event) const -> bool {
    return eventId == event.getEventId() && sequenceNum == event.getSequenceNum();
}

template<typename HANDLER_TYPE>
auto SequencedAsyncAction::doAddHandler(EventId eventId, uint32_t seqNum, HANDLER_TYPE handler) -> Status {
    auto status = SingleEventAsyncAction::addHandler(eventId, handler);
    if(status != Result::CONTINUE) return status;

    this->sequenceNum = seqNum;

    return Result::CONTINUE;
}

auto SequencedAsyncAction::addHandler(EventId eventId, uint32_t seqNum, details::NormalFunction handler) -> Status {
    return doAddHandler(eventId, seqNum, handler);
}

auto SequencedAsyncAction::addHandler(EventId eventId, uint32_t seqNum, details::MemberFunction handler) -> Status {
    return doAddHandler(eventId, seqNum, handler);
}

TSL_NS_END