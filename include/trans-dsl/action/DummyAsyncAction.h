//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_DUMMYASYNCACTION_H
#define TRANS_DSL_2_DUMMYASYNCACTION_H

#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/domain/Event.h>
#include <functional>

TSL_NS_BEGIN

struct TransactionInfo;

namespace details {
    struct DummyClass{};
   struct DummyMsgType {};
   //using DummyEventHandler = Status (DummyAsyncAction::*)(TransactionInfo const&, Event const&);
   using NormalFunction = Status (*)(TransactionInfo const&, DummyMsgType const&);
   using MemberFunction = Status (*)(DummyClass const*, TransactionInfo const&, DummyMsgType const&);
}

TSL_NS_END

#endif //TRANS_DSL_2_DUMMYASYNCACTION_H
