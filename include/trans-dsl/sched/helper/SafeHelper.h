//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_SAFEHELPER_H
#define TRANS_DSL_2_SAFEHELPER_H

#include <trans-dsl/sched/action/SchedSafe.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/helper/AutoActionHelper.h>

TSL_NS_BEGIN

namespace details {
   template<CONCEPT(SchedActionConcept) T_ACTION>
   struct Safe final : SchedSafe {
      using ThreadActionCreator = ThreadCreator_t<T_ACTION>;
   private:
      CONCEPT_ASSERT(SchedActionConcept<T_ACTION>);
      IMPL_ROLE_WITH_VAR(SchedAction, action);
      T_ACTION action;
   };
}

TSL_NS_END

#define __safe(...) TSL_NS::details::Safe_<TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>>

#endif //TRANS_DSL_2_SAFEHELPER_H
