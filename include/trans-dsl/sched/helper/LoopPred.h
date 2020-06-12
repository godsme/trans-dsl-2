//
// Created by Darwin Yuan on 2020/6/12.
//

#ifndef TRANS_DSL_2_LOOPPRED_H
#define TRANS_DSL_2_LOOPPRED_H

#include <trans-dsl/sched/concept/TransactionContext.h>
#include <trans-dsl/sched/action/SchedOptional.h>
#include <type_traits>
#include <cub/base/IsClass.h>
#include <algorithm>
#include <trans-dsl/sched/helper/Pred.h>

TSL_NS_BEGIN

namespace details {
   template<typename SIGNATURE>
   struct LoopPred {
      template<Status V_RESULT>
      struct PredBase : SIGNATURE {
         enum : Status {
            FinalResult = V_RESULT
         };
      };

      template<typename T, Status V_RESULT = Result::UNSPECIFIED, size_t V_SIZE = sizeof(T), typename = void>
      struct ClassPred;

      template<typename T, Status V_RESULT, size_t V_SIZE>
      struct ClassPred<T, V_RESULT, V_SIZE, CUB_NS::IsClass<T>>
         : PredBase<V_RESULT> {
         auto operator()(const TransactionInfo& context) -> bool {
            return pred(context);
         }

      private:
         T pred;
      };

      template<typename T, Status V_RESULT>
      struct ClassPred<T, V_RESULT, 1, CUB_NS::IsClass<T>>
         : PredBase<V_RESULT> {
         auto operator()(const TransactionInfo& context) -> bool {
            return T{}(context);
         }
      };

      template<PredFunction V_FUNC, Status V_RESULT>
      struct FuncPred : PredBase<V_RESULT> {
         auto operator()(const TransactionInfo& context) -> bool {
            return V_FUNC(context);
         }
      };

      template<typename T, Status V_RESULT = Result::UNSPECIFIED>
      static auto DeduceType() -> ClassPred<T, V_RESULT>;

      template<PredFunction V_FUNC, Status V_RESULT = Result::UNSPECIFIED>
      static auto DeduceType() -> FuncPred<V_FUNC, V_RESULT>;
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_LOOPPRED_H
