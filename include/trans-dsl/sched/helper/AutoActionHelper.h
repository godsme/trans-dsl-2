//
// Created by Darwin Yuan on 2020/7/6.
//

#ifndef TRANS_DSL_2_AUTOACTIONHELPER_H
#define TRANS_DSL_2_AUTOACTIONHELPER_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/helper/ProcedureHelper.h>

TSL_NS_BEGIN

namespace details {
   class AutoAction final {
      template<typename ... Tss>
      struct FinalTrait;

      template<typename T>
      struct FinalTrait<T> {
         using type = T;
      };

      template<typename ... Ts>
      constexpr static bool IsProcedure = FinallyConcept<TakeRight_tt<1, FinalTrait, Ts...>>;

      template<typename = void, typename ... Ts>
      struct ProcedureTrait {
         using type = __sequential(Ts...);
      };

      template<typename ... Ts>
      struct ProcedureTrait<std::enable_if_t<IsProcedure<Ts...>>, Ts...> {
         using type = __procedure(Ts...);
      };

   public:
      template<typename ... Ts>
      struct SequentialTrait {
         static_assert(sizeof...(Ts) > 0, "should not be empty");
         using type = typename ProcedureTrait<void, Ts...>::type;
      };

      template<typename  T>
      struct SequentialTrait<T> {
         using type = T;
      };

      template<typename ... Ts>
      using SequentialTrait_t = typename SequentialTrait<Ts...>::type;
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_AUTOACTIONHELPER_H
