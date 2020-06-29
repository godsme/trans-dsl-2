//
// Created by Darwin Yuan on 2020/6/13.
//

#ifndef TRANS_DSL_2_CONCURRENTHELPER_H
#define TRANS_DSL_2_CONCURRENTHELPER_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/sched/action/SchedConcurrent.h>
#include <trans-dsl/sched/helper/IsSchedAction.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/sched/helper/TypeExtractor.h>
#include <cub/utils/RepeatMacros.h>

TSL_NS_BEGIN

struct SchedAction;

namespace details {
   template<typename ... T_ACTIONS>
   struct GenericConcurrent;

   template<typename T_HEAD, typename ... T_TAIL>
   struct GenericConcurrent<T_HEAD, T_TAIL...> : GenericConcurrent<T_TAIL...> {
      auto get() -> SchedAction * {
         return &action;
      }
   private:
      T_HEAD action;
   };

   template<>
   struct GenericConcurrent<> {
      auto get() -> SchedAction * {
         return nullptr;
      }
   };

   template<CONCEPT(SchedActionConcept) ... T_ACTIONS>
   struct Concurrent : SchedConcurrent, private GenericConcurrent<T_ACTIONS...> {
      static constexpr size_t Num_Of_Actions = sizeof...(T_ACTIONS);
      static constexpr size_t Max_Num_Of_Actions = SchedConcurrent::Max_Num_Of_Children;

      template<typename ... Ts>
      struct LoopElem {
         using Type = GenericConcurrent<Ts...>;
      };

      template <SeqInt N>
      auto get() -> SchedAction* {
         if constexpr(N < Num_Of_Actions) {
            return TypeExtractor_t<N, LoopElem, T_ACTIONS...>::get();
         } else {
            return nullptr;
         }
      }

      private:
         OVERRIDE(getNumOfActions() const -> SeqInt) {
            return Num_Of_Actions;
         }

///////////////////////////////////////////////////////////////////////
#define CoNcUrReNt_GeT_AcTiOn__(n) case n: return get<n>();
#define CoNcUrReNt_AcTiOn_DeCl(n) if constexpr(Num_Of_Actions <= n) { \
   switch (seq) { SIMPLE_REPEAT(n, CoNcUrReNt_GeT_AcTiOn__) } \
}
#define And_CoNcUrReNt_AcTiOn_DeCl(n) else CoNcUrReNt_AcTiOn_DeCl(n)

         OVERRIDE(get(SeqInt seq) -> SchedAction*) {
            CoNcUrReNt_AcTiOn_DeCl(2)
            And_CoNcUrReNt_AcTiOn_DeCl(3)
            And_CoNcUrReNt_AcTiOn_DeCl(4)
            And_CoNcUrReNt_AcTiOn_DeCl(5)
            And_CoNcUrReNt_AcTiOn_DeCl(6)
            return nullptr;
         }

      static_assert(Num_Of_Actions >= 2, "# of concurrent actions should be at least 2");
      static_assert(Num_Of_Actions <= Max_Num_Of_Actions, "too much actions in __concurrent");
   };
}

#define __concurrent(...) TSL_NS::details::Concurrent<__VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_CONCURRENTHELPER_H
