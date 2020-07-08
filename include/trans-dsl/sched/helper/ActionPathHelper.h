//
// Created by Darwin Yuan on 2020/6/29.
//

#ifndef TRANS_DSL_2_ACTIONPATHHELPER_H
#define TRANS_DSL_2_ACTIONPATHHELPER_H

#include <trans-dsl/sched/action/ActionPath.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/sched/helper/Pred.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/helper/AutoActionHelper.h>
#include <algorithm>

TSL_NS_BEGIN

namespace details {

   template<TransListenerObservedAids const& AIDs, typename T_ACTION>
   struct ActionPathBase : ActionPath {
   protected:
      using Action = ActionRealTypeTraits_t<AIDs, T_ACTION>;
   public:
      using ThreadActionCreator = ThreadCreator_t<Action>;
   };

   template<typename T_PRED, typename T_ACTION>
   struct ActionPathClass final {
      template<TransListenerObservedAids const& AIDs>
      class ActionRealType : public ActionPathBase<AIDs, T_ACTION> {
         using Action = typename ActionPathBase<AIDs, T_ACTION>::Action;

         OVERRIDE(shouldExecute(TransactionInfo const& trans) noexcept -> bool) {
            return (*(new(cache) T_PRED))(trans);
         }

         OVERRIDE(getAction() noexcept -> SchedAction&) {
            return *(new(cache) Action);
         }

      private:
         // for any action path, both pred & action would not
         // be constructed until having to.
         alignas(std::max(alignof(T_PRED), alignof(Action)))
         char cache[std::max(sizeof(T_PRED), sizeof(Action))];
      };
   };

   template<PredFunction V_PRED, typename T_ACTION>
   struct ActionPathFunc final  {
      template<TransListenerObservedAids const& AIDs>
      class ActionRealType : public ActionPathBase<AIDs, T_ACTION> {
         using Action = typename ActionPathBase<AIDs, T_ACTION>::Action;

         OVERRIDE(shouldExecute(TransactionInfo const& trans) noexcept -> bool) {
            return V_PRED(trans);
         }

         OVERRIDE(getAction() noexcept -> SchedAction&) {
            return *(new(cache) Action);
         }

      private:
         // for any action path, the action would not
         // be constructed until having to.
         alignas(Action) char cache[sizeof(Action)];
      };
   };

   /////////////////////////////////////////////////////////////////////////
   template<typename T_PRED, typename T_ACTION>
   auto DeduceActionPath() -> ActionPathClass<T_PRED, T_ACTION>;

   template<PredFunction V_PRED, typename T_ACTION>
   auto DeduceActionPath() -> ActionPathFunc<V_PRED, T_ACTION>;

   /////////////////////////////////////////////////////////////////////////
   inline auto IsTrue__(TransactionInfo const&) -> bool { return true; }
}

TSL_NS_END

#define __case(pred, ...) \
   decltype(TSL_NS::details::DeduceActionPath<pred, TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>>())
#define __otherwise(...) __case(TSL_NS::details::IsTrue__, __VA_ARGS__)

#endif //TRANS_DSL_2_ACTIONPATHHELPER_H
