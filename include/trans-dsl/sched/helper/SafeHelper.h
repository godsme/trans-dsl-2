//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_SAFEHELPER_H
#define TRANS_DSL_2_SAFEHELPER_H

#include <trans-dsl/sched/action/SchedSafe.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/helper/AutoActionHelper.h>
#include <trans-dsl/sched/helper/ActionRealTypeTraits.h>

TSL_NS_BEGIN

namespace details {
   template<typename T_ACTION>
   struct Safe : SchedSafe {
      template<const TransListenerObservedAids& AIDs>
      class ActionRealType : SchedSafe {
         using Action = ActionRealTypeTraits_t<AIDs, T_ACTION>;
      public:
         using ThreadActionCreator = ThreadCreator_t<Action>;
         CONCEPT_ASSERT(SchedActionConcept<Action>);
      private:
         IMPL_ROLE_WITH_VAR(SchedAction, action);
         Action action;
      };
   };
}

TSL_NS_END

#define __safe(...) TSL_NS::details::Safe_<TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>>

#endif //TRANS_DSL_2_SAFEHELPER_H
