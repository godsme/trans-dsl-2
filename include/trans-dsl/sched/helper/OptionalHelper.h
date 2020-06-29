//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_OPTIONALHELPER_H
#define TRANS_DSL_2_OPTIONALHELPER_H

#include <trans-dsl/sched/action/SchedOptional.h>
#include <cub/base/IsClass.h>
#include <trans-dsl/sched/helper/Pred.h>

TSL_NS_BEGIN

namespace details {

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
   struct OptionalFunction final : OptionalBase<T_ACTION> {
   private:
      OVERRIDE(isTrue(TransactionContext& context) -> bool) {
         return V_PRED(context);
      }
   };

   ////////////////////////////////////////////////////////////////
   template<typename T_PRED, typename T_ACTION, size_t V_SIZE = sizeof(T_PRED), typename = void>
   struct OptionalClass;

   ////////////////////////////////////////////////////////////////
   template<typename T_PRED, typename T_ACTION, size_t V_SIZE>
   struct OptionalClass<T_PRED, T_ACTION, V_SIZE, CUB_NS::IsClass<T_PRED>> final : OptionalBase<T_ACTION> {
   private:
      OVERRIDE(isTrue(TransactionContext& context) -> bool) {
         return pred(context);
      }

   private:
      T_PRED pred;
   };

   template<typename T_PRED, typename T_ACTION>
   struct OptionalClass<T_PRED, T_ACTION, 1, CUB_NS::IsClass<T_PRED>> final : OptionalBase<T_ACTION> {
   private:
      OVERRIDE(isTrue(TransactionContext& context) -> bool) {
         return T_PRED{}(context);
      }
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
