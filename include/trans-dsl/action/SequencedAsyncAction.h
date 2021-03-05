//
// Created by godsme on 2021/3/5.
//

#ifndef TRANS_DSL_2_SEQUENCEDASYNCACTION_H
#define TRANS_DSL_2_SEQUENCEDASYNCACTION_H


#include <trans-dsl/action/SingleEventAsyncAction.h>
#include <trans-dsl/action/MsgHandlerTrait.h>

TSL_NS_BEGIN

struct TransactionInfo;

class SequencedAsyncAction : public SingleEventAsyncAction {
    template<typename HANDLER_TYPE>
    auto doAddHandler(EventId eventId, uint32_t seqNum, HANDLER_TYPE handler) -> Status;

    auto addHandler(EventId eventId, uint32_t seqNum, details::NormalFunction handler) -> Status;
    auto addHandler(EventId eventId, uint32_t seqNum, details::MemberFunction handler) -> Status;

    OVERRIDE(matches(Event const& event) const -> bool);

private:
    uint32_t sequenceNum{0};

protected:
    template<typename M>
    auto WAIT_ON(
        EventId eventId,
        uint32_t seqNum,
        M&& handler
    ) -> Status
    {
        if constexpr (MsgHandlerTrait<M>::IsNormalFunction) {
            return addHandler(eventId, seqNum, handler);
        } else {
            static_assert(sizeof(M) == 1);
            return addHandler(eventId, seqNum, extractP2MF(&M::operator()));
        }
    }
};

#define DEF_SEQ_ASYNC_ACTION(action) \
struct action : TSL_NS::SequencedAsyncAction

TSL_NS_END

#endif //TRANS_DSL_2_SEQUENCEDASYNCACTION_H
