//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_ACTIONPATH_H
#define TRANS_DSL_2_ACTIONPATH_H

#include <trans-dsl/tsl_ns.h>
#include <cub/dci/Role.h>

TSL_NS_BEGIN

struct TransactionInfo;
struct SchedAction;

DEFINE_ROLE(ActionPath) {
   ABSTRACT(shouldExecute(TransactionInfo const&) -> bool);
   ABSTRACT(getAction() -> SchedAction&);
};

TSL_NS_END

#endif //TRANS_DSL_2_ACTIONPATH_H
