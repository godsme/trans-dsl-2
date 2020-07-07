//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_THROWHELPER_H
#define TRANS_DSL_2_THROWHELPER_H

#include <trans-dsl/sched/action/SchedSyncAction.h>

TSL_NS_BEGIN

namespace details {
   template<Status V_CODE>
   class Throw : public SchedSyncAction {
      OVERRIDE(exec(TransactionContext&) -> Status) { return V_CODE; }
   };
}

TSL_NS_END

#define __throw(code) TSL_NS::details::Throw<code>

#endif //TRANS_DSL_2_THROWHELPER_H
