//
// Created by Darwin Yuan on 2020/7/7.
//

#ifndef TRANS_DSL_2_TRANSACTIONLISTENER_H
#define TRANS_DSL_2_TRANSACTIONLISTENER_H

#include <trans-dsl/tsl_ns.h>
#include <cub/dci/Role.h>
#include <trans-dsl/sched/domain/Event.h>
#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/domain/ActionId.h>

TSL_NS_BEGIN

struct TransactionInfo;

DEFINE_ROLE(TransactionListener) {
   virtual auto onActionStarting(ActionId, TransactionInfo const&) -> void {} // ++
   virtual auto onActionEventConsumed(ActionId, TransactionInfo const&, Event const&) -> void {} //++
   virtual auto onActionDone(ActionId, TransactionInfo const&, Status)  -> void {}               //++
   virtual auto onActionStartStopping(ActionId, TransactionInfo const&, Status) -> void  {}
   virtual auto onActionStopped(ActionId, TransactionInfo const&, Status)  -> void {}            //+
   virtual auto onActionKilled(ActionId, TransactionInfo const&, Status)  -> void {}       //+
};

TSL_NS_END

#endif //TRANS_DSL_2_TRANSACTIONLISTENER_H
