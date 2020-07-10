//
// Created by Darwin Yuan on 2020/6/16.
//

#ifndef TRANS_DSL_2_VOIDHELPER_H
#define TRANS_DSL_2_VOIDHELPER_H

#include <trans-dsl/sched/action/SchedVoid.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/sched/concepts/ConceptHelper.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/helper/AutoActionHelper.h>
#include <trans-dsl/sched/helper/ActionRealTypeTraits.h>

TSL_NS_BEGIN

namespace details {

   template<typename T_ACTION>
   struct Void final {
      template<TransListenerObservedAids const& AIDs>
      class ActionRealType : public SchedVoid {
         using Action = ActionRealTypeTraits_t<AIDs, T_ACTION>;
         CONCEPT_ASSERT(SchedActionConcept<Action>);

         IMPL_ROLE_WITH_VAR(SchedAction, action);
         Action action;

      public:
         using ThreadActionCreator = ThreadCreator_t<Action>;
      };
   };

   template<typename ACTION>
   using Void_t = typename Void<ACTION>::template ActionRealType<EmptyAids>;
}

TSL_NS_END

#define __void(...) TSL_NS::details::Void<TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>>
#define __def_void(...) TSL_NS::details::Void_t<TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>>

#endif //TRANS_DSL_2_VOIDHELPER_H
