//
// Created by godsme on 2021/3/5.
//

#ifndef TRANS_DSL_2_SEQUENCEDASYNCACTION_H
#define TRANS_DSL_2_SEQUENCEDASYNCACTION_H


#include <trans-dsl/action/SingleEventAsyncAction.h>
#include <trans-dsl/action/MsgHandlerTrait.h>

TSL_NS_BEGIN

struct TransactionInfo;

struct SequencedAsyncAction : SingleEventAsyncAction {
    auto exec(TransactionInfo const&) -> Status ;

private:
    OVERRIDE(matches(Event const& event) const -> bool);

private:
    ABSTRACT(doExec(TransactionInfo const&) -> Status);
    ABSTRACT(getSequenceNum(TransactionInfo const&) -> uint32_t);

private:
    uint32_t sequenceNum{0};
};

TSL_NS_END

#endif //TRANS_DSL_2_SEQUENCEDASYNCACTION_H
