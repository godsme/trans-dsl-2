//
// Created by Darwin Yuan on 2020/7/7.
//

#ifndef TRANS_DSL_2_WITHIDHELPER_H
#define TRANS_DSL_2_WITHIDHELPER_H

#include <trans-dsl/sched/action/SchedWithId.h>
#include <trans-dsl/sched/domain/TransListenerObservedAids.h>
#include <trans-dsl/sched/helper/ActionRealTypeTraits.h>

TSL_NS_BEGIN

namespace details {
   template<ActionId AID, typename ACTION>
   class WithId final  {
      template<TransListenerObservedAids const& AIDs>
      using Action = ActionRealTypeTraits_t<AIDs, ACTION>;

      template<TransListenerObservedAids const& AIDs>
      struct Inner : SchedWithId {
      private:
         OVERRIDE(getActionId() const -> ActionId) { return AID; }
         IMPL_ROLE_WITH_VAR(SchedAction, action);
         Action<AIDs> action;
      };

      template<TransListenerObservedAids const& AIDs, bool>
      struct Trait {
         using type = Action<AIDs>;
      };

      template<TransListenerObservedAids const& AIDs>
      struct Trait<AIDs, true>  {
         using type = Inner<AIDs>;
      };
   public:
      template<TransListenerObservedAids const& AIDs>
      struct ActionRealType : Trait<AIDs, AIDs.isEnabled(AID)>::type {
         using ThreadActionCreator = ThreadCreator_t<Action<AIDs>>;
      };
   };

   template<ActionId AID, typename ACTION>
   using WithId_t = typename WithId<AID, ACTION>::template ActionRealType<EmptyAids>;
}

TSL_NS_END

#define __with_id(aid, ...) TSL_NS::details::WithId<aid, TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>>
#define __def_with_id(aid, ...) TSL_NS::details::WithId_t<aid, TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>>


#endif //TRANS_DSL_2_WITHIDHELPER_H
