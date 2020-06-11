//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_OPTIONALHELPER_H
#define TRANS_DSL_2_OPTIONALHELPER_H

#include <trans-dsl/sched/concept/TransactionContext.h>
#include <trans-dsl/sched/action/SchedOptional.h>
#include <type_traits>
#include <cub/base/IsClass.h>

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
   template<typename T_PRED, typename T_ACTION, size_t V_SIZE = sizeof(T_PRED), typename = void>
   struct OptionalClass;

   ////////////////////////////////////////////////////////////////
   template<typename T_PRED, typename T_ACTION, size_t V_SIZE>
   struct OptionalClass<T_PRED, T_ACTION, V_SIZE, CUB_NS::IsClass<T_PRED>> : OptionalBase<T_ACTION> {
   private:
      OVERRIDE(isTrue(TransactionContext& context) -> bool) {
         return pred(context.ROLE(TransactionInfo));
      }

   private:
      T_PRED pred;
   };

   template<typename T_PRED, typename T_ACTION>
   struct OptionalClass<T_PRED, T_ACTION, 1, CUB_NS::IsClass<T_PRED>> : OptionalBase<T_ACTION> {
   private:
      OVERRIDE(isTrue(TransactionContext& context) -> bool) {
         return T_PRED{}(context.ROLE(TransactionInfo));
      }
   };

   ////////////////////////////////////////////////////////////////
   template<typename T, typename T_ACTION>
   auto deductOptionalClass() -> OptionalClass<T, T_ACTION>;

   template<PredFunction V_FUNC, typename T_ACTION>
   auto deductOptionalClass() -> OptionalFunction<V_FUNC, T_ACTION>;
}

#define __optional(...)   decltype(details::deductOptionalClass<__VA_ARGS__>())

TSL_NS_END

#endif //TRANS_DSL_2_OPTIONALHELPER_H
