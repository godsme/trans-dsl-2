//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_PRED_H
#define TRANS_DSL_2_PRED_H

#include <trans-dsl/tsl_ns.h>
#include <cub/base/IsClass.h>

TSL_NS_BEGIN

struct TransactionInfo;

using PredFunction = bool (*)(const TransactionInfo&);

namespace details {
   template<typename T_PRED, size_t V_SIZE=sizeof(T_PRED), typename = void>
   struct ClassNot__;

   template<typename T_PRED>
   struct ClassNot__<T_PRED, sizeof(T_PRED), CUB_NS::IsClass<T_PRED>> {
      bool operator()(const TransactionInfo& context) {
         return !pred(context);
      }

   private:
      T_PRED pred;
   };

   template<typename T_PRED>
   struct ClassNot__<T_PRED, 1, CUB_NS::IsClass<T_PRED>> {
      bool operator()(const TransactionInfo& context) {
         return !T_PRED{}(context);
      }
   };

   template<PredFunction V_FUNC>
   struct FuncNot__ {
      bool operator()(const TransactionInfo& context) {
         return !V_FUNC(context);
      }
   };

   template<typename T_PRED>
   auto DeduceNot() -> ClassNot__<T_PRED>;

   template<PredFunction V_FUNC>
   auto DeduceNot() -> FuncNot__<V_FUNC>;
}

#define __not(...) decltype(TSL_NS::details::DeduceNot<__VA_ARGS__>())

TSL_NS_END

#endif //TRANS_DSL_2_PRED_H
