//
// Created by Darwin Yuan on 2020/7/4.
//

#ifndef TRANS_DSL_2_TRANSACTIONHELPER_H
#define TRANS_DSL_2_TRANSACTIONHELPER_H

#include <trans-dsl/sched/action/Transaction.h>
#include <trans-dsl/sched/action/SchedMultiThread.h>

#define __transaction(...) TSL_NS::Transaction<__VA_ARGS__>
#define __mt_transaction(...) __transaction(GenericMultiThreadScheduler<__VA_ARGS__>)

#endif //TRANS_DSL_2_TRANSACTIONHELPER_H
