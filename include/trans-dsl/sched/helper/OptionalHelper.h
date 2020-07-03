//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_OPTIONALHELPER_H
#define TRANS_DSL_2_OPTIONALHELPER_H

#include <trans-dsl/sched/action/SchedOptional.h>
#include <cub/base/IsClass.h>
#include <trans-dsl/sched/helper/Pred.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/concepts/PredConcept.h>

TSL_NS_BEGIN

namespace details {

   ////////////////////////////////////////////////////////////////
   template<typename T_ACTION>
   struct OptionalBase : SchedOptional {
      using ThreadActionCreator = ThreadCreator_t<T_ACTION>;
      CONCEPT_ASSERT(SchedActionConcept<T_ACTION>);
   private:
      OVERRIDE(getAction() -> SchedAction*) { return new (cache) T_ACTION; }
   private:
      alignas(alignof(T_ACTION)) unsigned char cache[sizeof(T_ACTION)];
   };

   ////////////////////////////////////////////////////////////////
   template<PredFunction V_PRED, CONCEPT(SchedActionConcept) T_ACTION>
   class OptionalFunction final : public OptionalBase<T_ACTION> {
      OVERRIDE(isTrue(TransactionContext& context) -> bool) { return V_PRED(context); }
   };

   ////////////////////////////////////////////////////////////////
   template<CONCEPT(PredConcept) T_PRED, CONCEPT(SchedActionConcept) T_ACTION, size_t V_SIZE = sizeof(T_PRED)>
   class OptionalClass final : public OptionalBase<T_ACTION>, T_PRED {
      CONCEPT_ASSERT(PredConcept<T_PRED>);
      OVERRIDE(isTrue(TransactionContext& context) -> bool) { return T_PRED::operator()(context); }
   };

   ////////////////////////////////////////////////////////////////
   template<typename T, typename T_ACTION>
   auto deductOptionalClass__() -> OptionalClass<T, T_ACTION>;

   template<PredFunction V_FUNC, typename T_ACTION>
   auto deductOptionalClass__() -> OptionalFunction<V_FUNC, T_ACTION>;

   /////////////////////////////////////////////////////////////////////

   inline auto IsFailed__(TransactionInfo const& info) -> bool {
      return cub::is_failed_status(info.getStatus());
   }

   inline auto IsSucc__(TransactionInfo const& info) -> bool {
      return Result::SUCCESS == info.getStatus();
   }

   template<Status V_STATUS>
   inline auto IsStatus__(TransactionInfo const& info) -> bool {
      return info.getStatus() == V_STATUS;
   }
}

#define __is_succ TSL_NS::details::IsSucc__
#define __is_failed TSL_NS::details::IsFailed__
#define __is_status(status) TSL_NS::details::IsStatus__<status>
#define __optional(...)   decltype(TSL_NS::details::deductOptionalClass__<__VA_ARGS__>())
#define __on_fail(...) __optional(__is_failed, __VA_ARGS__)
#define __on_status(status, ...) __optional(__is_status(status), __VA_ARGS__)


TSL_NS_END

#endif //TRANS_DSL_2_OPTIONALHELPER_H
