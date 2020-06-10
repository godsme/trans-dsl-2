//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_DUMMYASYNCACTION_H
#define TRANS_DSL_2_DUMMYASYNCACTION_H

#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/concept/Event.h>

TSL_NS_BEGIN

struct TransactionInfo;

namespace details {
   struct DummyAsyncAction {};
   using DummyEventHandler = Status (DummyAsyncAction::*)(const TransactionInfo&, const Event&);
   using NormalFunction = Status (*)(DummyAsyncAction*, const TransactionInfo&, const Event&);
}

TSL_NS_END

#endif //TRANS_DSL_2_DUMMYASYNCACTION_H
