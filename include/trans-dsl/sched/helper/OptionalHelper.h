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
#include <trans-dsl/sched/helper/AutoActionHelper.h>
#include <trans-dsl/sched/helper/ActionRealTypeTraits.h>

TSL_NS_BEGIN

namespace details {

   ////////////////////////////////////////////////////////////////
   template<typename T_ACTION>
   struct OptionalBase {
      template<const TransListenerObservedAids& AIDs>
      class Inner : public SchedOptional {
         using Action = ActionRealTypeTraits_t<AIDs, T_ACTION>;
         OVERRIDE(getAction() -> SchedAction*) { return new (cache) Action; }
         alignas(alignof(Action)) unsigned char cache[sizeof(Action)];
      public:
         using ThreadActionCreator = ThreadCreator_t<Action>;
      };
   };

   ////////////////////////////////////////////////////////////////
   template<PredFunction V_PRED, typename T_ACTION>
   class OptionalFunction final {
      template<const TransListenerObservedAids& AIDs>
      using Base = typename OptionalBase<T_ACTION>::template Inner<AIDs>;

   public:
      template<const TransListenerObservedAids& AIDs>
      struct ActionRealType : public Base<AIDs> {
         using ThreadActionCreator = typename  Base<AIDs>::ThreadActionCreator;
      public:
         OVERRIDE(isTrue(TransactionContext& context) -> bool) { return V_PRED(context); }
      };
   };

   ////////////////////////////////////////////////////////////////
   template<CONCEPT(PredConcept) T_PRED, typename T_ACTION>
   class OptionalClass final {
      CONCEPT_ASSERT(PredConcept<T_PRED>);

   public:
      template<const TransListenerObservedAids& AIDs>
      class ActionRealType : T_PRED, public OptionalBase<T_ACTION>::template Inner<AIDs> {
         OVERRIDE(isTrue(TransactionContext& context) -> bool) { return T_PRED::operator()(context); }
      };
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

#define __on_fail(...) __optional(__is_failed, __VA_ARGS__)
#define __on_status(status, ...) __optional(__is_status(status), __VA_ARGS__)

#define __optional(pred, ...)  \
decltype(TSL_NS::details::deductOptionalClass__<pred, TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>>())

#define __def_optional(pred, ...)  typename __optional(pred, __VA_ARGS__)::template ActionRealType<EmptyAids>

TSL_NS_END

#endif //TRANS_DSL_2_OPTIONALHELPER_H
