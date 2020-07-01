//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_SAFEHELPER_H
#define TRANS_DSL_2_SAFEHELPER_H

#include <trans-dsl/sched/action/SchedSafe.h>

TSL_NS_BEGIN

namespace details {
   template<CONCEPT_C(SchedActionConcept, T_ACTION)>
   class Safe final : SchedSafe {
      CONCEPT_ASSERT(SchedActionConcept<T_ACTION>);
      IMPL_ROLE_WITH_VAR(SchedAction, action);
      T_ACTION action;
   };
}

TSL_NS_END

#define __safe(...) TSL_NS::details::Safe_<__VA_ARGS__>

#endif //TRANS_DSL_2_SAFEHELPER_H
