//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_RUNTIMECONTEXT_H
#define TRANS_DSL_2_RUNTIMECONTEXT_H

#include <cub/dci/Role.h>
#include <trans-dsl/tsl_status.h>

TSL_NS_BEGIN

DEFINE_ROLE(RuntimeContext) {
   ABSTRACT(reportFailure(Status) -> Status);
   ABSTRACT(getStatus() const     -> Status);
};

TSL_NS_END

#endif //TRANS_DSL_2_RUNTIMECONTEXT_H
