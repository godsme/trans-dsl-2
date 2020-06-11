//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_OPTIONALHELPER_H
#define TRANS_DSL_2_OPTIONALHELPER_H

#include <trans-dsl/sched/concept/TransactionContext.h>
#include <trans-dsl/sched/action/SchedOptional.h>
#include <type_traits>

TSL_NS_BEGIN

namespace details {
   using PredFunction = bool (*)(const TransactionInfo&);

   ////////////////////////////////////////////////////////////////
   template<typename T_ACTION>
   struct OptionalBase : SchedOptional {
      OVERRIDE(getAction() -> SchedAction*) {
         return new (cache) T_ACTION;
      }

   private:
      alignas(alignof(T_ACTION)) unsigned char cache[sizeof(T_ACTION)];
   };

   ////////////////////////////////////////////////////////////////
   template<PredFunction V_PRED, typename T_ACTION>
   struct OptionalFunction : OptionalBase<T_ACTION> {
   private:
      OVERRIDE(isTrue(TransactionContext& context) -> bool) {
         return V_PRED(context.ROLE(TransactionInfo));
      }
   };

   ////////////////////////////////////////////////////////////////
   template<typename T_PRED, typename T_ACTION>
   struct OptionalClass : OptionalBase<T_ACTION> {
   private:
      OVERRIDE(isTrue(TransactionContext& context) -> bool) {
         return pred(context.ROLE(TransactionInfo));
      }

   private:
      T_PRED pred;
   };

   ////////////////////////////////////////////////////////////////
   template<typename T, typename T_ACTION, std::enable_if_t<std::is_class_v<T>, int> = 0>
   auto deductOptionalClass() -> OptionalClass<T, T_ACTION>;

   template<PredFunction V_FUNC, typename T_ACTION>
   auto deductOptionalClass() -> OptionalFunction<V_FUNC, T_ACTION>;
}

#define __optional(...)   decltype(details::deductOptionalClass<__VA_ARGS__>())

TSL_NS_END

#endif //TRANS_DSL_2_OPTIONALHELPER_H
