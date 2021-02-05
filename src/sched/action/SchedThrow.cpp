//
// Created by godsme on 2021/2/5.
//

#include <trans-dsl/sched/action/SchedThrow.h>
#include <trans-dsl/sched/domain/TransactionContext.h>

TSL_NS_BEGIN

auto SchedThrow::exec(TransactionContext & trans) -> Status {
    auto result = getStatus();
    return (result == Result::UNSPECIFIED) ? static_cast<TransactionInfo&>(trans).getStatus() : result;
}

TSL_NS_END