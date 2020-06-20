//
// Created by Darwin Yuan on 2020/6/16.
//

#ifndef TRANS_DSL_2_ACTIONCONCEPT_H
#define TRANS_DSL_2_ACTIONCONCEPT_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/domain/Event.h>
#include <trans-dsl/tsl_status.h>
#include <concepts>

TSL_NS_BEGIN

struct TransactionInfo;

template<typename T>
concept ActionConcept = requires
        ( T t,
          TransactionInfo& trans,
          const Event& event,
          Status cause) {
    { t.exec(trans) }               -> std::same_as<Status>;
    { t.handleEvent(trans, event) } -> std::same_as<Status>;
    { t.kill(trans, cause) }        -> std::same_as<void>;
};

TSL_NS_END

#endif //TRANS_DSL_2_ACTIONCONCEPT_H
