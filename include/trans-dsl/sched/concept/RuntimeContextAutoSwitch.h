//
// Created by Darwin Yuan on 2020/6/12.
//

#ifndef TRANS_DSL_2_RUNTIMECONTEXTAUTOSWITCH_H
#define TRANS_DSL_2_RUNTIMECONTEXTAUTOSWITCH_H

#include <trans-dsl/sched/concept/TransactionContext.h>

TSL_NS_BEGIN

struct RuntimeContextAutoSwitch {
   RuntimeContextAutoSwitch(TransactionContext& context, RuntimeContext& thisContext)
      : parentKeeper(context.getRuntimeContext())
      , contextInfo(context) {
      contextInfo.setRuntimeContext(thisContext);
   }

   ~RuntimeContextAutoSwitch() {
      contextInfo.setRuntimeContext(parentKeeper);
   }

   RuntimeContext& parentKeeper;
   RuntimeContextInfo& contextInfo;
};

TSL_NS_END

#endif //TRANS_DSL_2_RUNTIMECONTEXTAUTOSWITCH_H
