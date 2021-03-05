//
// Created by godsme on 2021/3/5.
//

#include <trans-dsl/action/SequencedAsyncAction.h>

TSL_NS_BEGIN

auto SequencedAsyncAction::matchesMore(Event const& event) const -> bool {
    return sequenceNum == event.getSequenceNum();
}

auto SequencedAsyncAction::exec(TransactionInfo const& trans) -> Status {
    auto status = doExec(trans);
    if(status != Result::CONTINUE) {
        return status;
    }

    sequenceNum = getSequenceNum(trans);

    return Result::CONTINUE;
}

TSL_NS_END