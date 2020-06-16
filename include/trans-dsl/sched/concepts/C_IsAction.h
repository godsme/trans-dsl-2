//
// Created by Darwin Yuan on 2020/6/16.
//

#ifndef TRANS_DSL_2_C_ISACTION_H
#define TRANS_DSL_2_C_ISACTION_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/domain/Event.h>
#include <trans-dsl/tsl_status.h>
#include <concepts>

TSL_NS_BEGIN
struct TransactionInfo;

template<typename T>
concept C_IsAction = requires (T t, TransactionInfo& trans, const Event& event, Status cause) {
   { t.exec(trans) } -> std::same_as<Status>;
};

TSL_NS_END

#endif //TRANS_DSL_2_C_ISACTION_H
