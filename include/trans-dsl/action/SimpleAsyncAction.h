//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_SIMPLEASYNCACTION_H
#define TRANS_DSL_2_SIMPLEASYNCACTION_H

#include <trans-dsl/action/SingleEventAsyncAction.h>
#include <trans-dsl/action/MsgHandlerTrait.h>

TSL_NS_BEGIN

struct TransactionInfo;

class SimpleAsyncAction : public SingleEventAsyncAction {};

#define DEF_SIMPLE_ASYNC_ACTION(action) \
struct action : TSL_NS::SimpleAsyncAction

TSL_NS_END

#endif //TRANS_DSL_2_SIMPLEASYNCACTION_H
