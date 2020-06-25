//
// Created by Darwin Yuan on 2020/6/12.
//

#ifndef TRANS_DSL_2_LOOPPRED_H
#define TRANS_DSL_2_LOOPPRED_H

#include <trans-dsl/sched/helper/Pred.h>

TSL_NS_BEGIN

namespace details {
   struct LoopPredSignature {};

   template<Status V_RESULT>
   struct LoopPredBase : LoopPredSignature {
      enum : Status {
         FinalResult = V_RESULT
      };
   };

   template<typename T, Status V_RESULT = Result::UNSPECIFIED, size_t V_SIZE = sizeof(T), typename = void>
   struct LoopPredClassPred;

   template<typename T, Status V_RESULT, size_t V_SIZE>
   struct LoopPredClassPred<T, V_RESULT, V_SIZE, CUB_NS::IsClass<T>>
      : LoopPredBase<V_RESULT> {
      auto operator()(TransactionInfo const& context) -> bool {
         return pred(context);
      }

   private:
      T pred;
   };

   template<typename T, Status V_RESULT>
   struct LoopPredClassPred<T, V_RESULT, 1, CUB_NS::IsClass<T>>
      : LoopPredBase<V_RESULT> {
      auto operator()(TransactionInfo const& context) -> bool {
         return T{}(context);
      }
   };

   template<PredFunction V_FUNC, Status V_RESULT>
   struct LoopPredFuncPred : LoopPredBase<V_RESULT> {
      auto operator()(TransactionInfo const& context) -> bool {
         return V_FUNC(context);
      }
   };

   template<typename T, Status V_RESULT = Result::UNSPECIFIED>
   static auto DeduceLoopPredType() -> LoopPredClassPred<T, V_RESULT>;

   template<PredFunction V_FUNC, Status V_RESULT = Result::UNSPECIFIED>
   static auto DeduceLoopPredType() -> LoopPredFuncPred<V_FUNC, V_RESULT>;
}

TSL_NS_END

#endif //TRANS_DSL_2_LOOPPRED_H
