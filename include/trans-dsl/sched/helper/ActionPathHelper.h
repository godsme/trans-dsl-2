//
// Created by Darwin Yuan on 2020/6/29.
//

#ifndef TRANS_DSL_2_ACTIONPATHHELPER_H
#define TRANS_DSL_2_ACTIONPATHHELPER_H

#include <trans-dsl/sched/action/ActionPath.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/sched/helper/Pred.h>
#include <algorithm>
#include <trans-dsl/utils/ThreadActionTrait.h>

TSL_NS_BEGIN

namespace details {

   template<typename T_PRED, CONCEPT(SchedActionConcept) T_ACTION>
   struct ActionPathClass final : ActionPath {
      using ThreadActionCreator = ThreadCreator_t<T_ACTION>;
   private:
      CONCEPT_ASSERT(SchedActionConcept<T_ACTION>);

      OVERRIDE(shouldExecute(TransactionInfo const& trans) noexcept -> bool) {
         return (*(new (cache) T_PRED))(trans);
      }

      OVERRIDE(getAction() noexcept -> SchedAction&) {
         return *(new (cache) T_ACTION);
      }

   private:
      // for any action path, both pred & action would not
      // be constructed until having to.
      alignas(std::max(alignof(T_PRED), alignof(T_ACTION)))
      char cache[std::max(sizeof(T_PRED), sizeof(T_ACTION))];
   };

   template<PredFunction V_PRED, typename T_ACTION>
   class ActionPathFunc final : public ActionPath {
      OVERRIDE(shouldExecute(TransactionInfo const& trans) noexcept -> bool) {
         return V_PRED(trans);
      }

      OVERRIDE(getAction() noexcept -> SchedAction&) {
         return *(new (cache) T_ACTION);
      }

   private:
      // for any action path, the action would not
      // be constructed until having to.
      alignas(T_ACTION) char cache[sizeof(T_ACTION)];
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

#define __case(pred, ...) decltype(TSL_NS::details::DeduceActionPath<pred, __VA_ARGS__>())
#define __otherwise(...) __case(TSL_NS::details::IsTrue__, __VA_ARGS__)

#endif //TRANS_DSL_2_ACTIONPATHHELPER_H
