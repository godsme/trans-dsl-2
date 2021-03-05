//
// Created by godsme on 2021/3/5.
//

#ifndef TRANS_DSL_2_SINGLEEVENTASYNCACTION_H
#define TRANS_DSL_2_SINGLEEVENTASYNCACTION_H

#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/domain/EventId.h>
#include <trans-dsl/action/DummyAsyncAction.h>
#include <trans-dsl/action/MsgHandlerTrait.h>
#include <trans-dsl/action/P2MFExtractor.h>
#include <variant>

TSL_NS_BEGIN

struct TransactionInfo;

struct SingleEventAsyncAction {
    auto handleEvent(TransactionInfo const& trans, Event const& event) -> Status;
    auto kill(TransactionInfo const&, Status) -> Status;

    virtual ~SingleEventAsyncAction() = default;
private:
    DEFAULT(matches(Event const&) const -> bool);

    auto reset() -> void;

    template<typename HANDLER_TYPE>
    auto doAddHandler(EventId eventId, HANDLER_TYPE handler) -> Status;

protected:
    auto addHandler(EventId eventId, details::NormalFunction handler) -> Status;
    auto addHandler(EventId eventId, details::MemberFunction handler) -> Status;

protected:
    std::variant<std::monostate, details::MemberFunction, details::NormalFunction> handler;
    EventId eventId{INVALID_EVENT_ID};

protected:
    template<typename M>
    auto WAIT_ON(
            EventId eventId,
            M&& handler
    ) -> Status
    {
        if constexpr (MsgHandlerTrait<M>::IsNormalFunction) {
            return addHandler(eventId, handler);
        } else {
            static_assert(sizeof(M) == 1);
            return addHandler(eventId, extractP2MF(&M::operator()));
        }
    }
};

#define MSG_HANDLER(msg_type) []([[maybe_unused]] TSL_NS::TransactionInfo const& trans, [[maybe_unused]] msg_type const& msg) -> TSL_NS::Status

TSL_NS_END

#endif //TRANS_DSL_2_SINGLEEVENTASYNCACTION_H
