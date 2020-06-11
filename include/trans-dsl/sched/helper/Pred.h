//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_PRED_H
#define TRANS_DSL_2_PRED_H

#include <trans-dsl/tsl_ns.h>

TSL_NS_BEGIN

struct TransactionInfo;

using PredFunction = bool (*)(const TransactionInfo&);

TSL_NS_END

#endif //TRANS_DSL_2_PRED_H
