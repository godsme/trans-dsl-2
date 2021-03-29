//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_THROWHELPER_H
#define TRANS_DSL_2_THROWHELPER_H

#include <trans-dsl/sched/action/SchedThrow.h>

TSL_NS_BEGIN

namespace details {
   template<Status V_CODE = Result::UNSPECIFIED>
   struct Throw {
       template<TransListenerObservedAids const& AIDs>
       class ActionRealType :  public SchedThrow {
           OVERRIDE(getStatus() const -> Status) {
               return V_CODE;
           }
       };
   };
}

TSL_NS_END

#define __throw(...) TSL_NS::details::Throw<__VA_ARGS__>

#endif //TRANS_DSL_2_THROWHELPER_H
