//
// Created by Darwin Yuan on 2020/6/13.
//

#ifndef TRANS_DSL_2_CONCURRENTHELPER_H
#define TRANS_DSL_2_CONCURRENTHELPER_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/sched/action/SchedConcurrent.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <cub/utils/RepeatMacros.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/domain/TransListenerObservedAids.h>

TSL_NS_BEGIN

struct SchedAction;

namespace details {
   template<typename ... T_ACTIONS>
   struct GenericConcurrent {
      auto get() -> SchedAction * {
         return nullptr;
      }
   };

   template<typename T_HEAD, typename ... T_TAIL>
   struct GenericConcurrent<T_HEAD, T_TAIL...> : GenericConcurrent<T_TAIL...> {
      auto get() -> SchedAction * {
         return &action;
      }
   private:
      T_HEAD action;
   };


   template<typename ... T_ACTIONS>
   class Concurrent final  {
      static constexpr size_t Num_Of_Actions = sizeof...(T_ACTIONS);
      static_assert(Num_Of_Actions >= 2, "# of concurrent actions should be at least 2");

      template<typename ... Tss>
      struct Base : private GenericConcurrent<Tss...> {
         template<SeqInt N>
         auto get() -> SchedAction * {
            if constexpr(N < Num_Of_Actions) {
               return CUB_NS::Drop_t<N, GenericConcurrent, Tss...>::get();
            } else {
               return nullptr;
            }
         }

         using ThreadActionCreator = ThreadCreator_t<Tss...>;
      };

      template<TransListenerObservedAids const& AIDs>
      struct Traits {
         template<typename T>
         using ToActionRealType = ActionRealTypeTraits<AIDs, T>;

         using Base = CUB_NS::Transform_t<ToActionRealType, Base, T_ACTIONS...>;
      };

   public:
      template<TransListenerObservedAids const& AIDs>
      class ActionRealType : public SchedConcurrent, Traits<AIDs>::Base {
         static constexpr size_t Max_Num_Of_Actions = SchedConcurrent::Max_Num_Of_Children;
         static_assert(Num_Of_Actions <= Max_Num_Of_Actions, "too much actions in __concurrent");

         using Base = typename Traits<AIDs>::Base;
      public:
         using ThreadActionCreator = typename Traits<AIDs>::Base::ThreadActionCreator;

      private:
         OVERRIDE(getNumOfActions() const -> SeqInt) {
            return Num_Of_Actions;
         }

         ///////////////////////////////////////////////////////////////////////
         #define CoNcUrReNt_GeT_AcTiOn__(n) case n: return Base::template get<n>();
         #define CoNcUrReNt_AcTiOn(n) { switch (seq) { SIMPLE_REPEAT(n, CoNcUrReNt_GeT_AcTiOn__) } }
         #define CoNcUrReNt_AcTiOn_DeCl(n) if constexpr(Num_Of_Actions <= n) CoNcUrReNt_AcTiOn(n)
         #define And_CoNcUrReNt_AcTiOn_DeCl(n) else if constexpr(Num_Of_Actions == n) CoNcUrReNt_AcTiOn(n)
         ///////////////////////////////////////////////////////////////////////

         // Use if-constexpr to avoid unnecessary function template instantiation.
         // Use switch-case to avoid recursion, and the generated jump-table by
         // switch-case is fast.
         OVERRIDE(get(SeqInt seq) -> SchedAction*) {
            CoNcUrReNt_AcTiOn_DeCl(2)
            And_CoNcUrReNt_AcTiOn_DeCl(3)
            And_CoNcUrReNt_AcTiOn_DeCl(4)
            And_CoNcUrReNt_AcTiOn_DeCl(5)
            And_CoNcUrReNt_AcTiOn_DeCl(6)
            return nullptr;
         }
      };
   };

   template<typename ... Ts>
   using Concurrent_t = typename Concurrent<Ts...>::template ActionRealType<EmptyAids>;
}

#define __concurrent(...) TSL_NS::details::Concurrent<__VA_ARGS__>
#define __def_concurrent(...) TSL_NS::details::Concurrent_t<__VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_CONCURRENTHELPER_H
