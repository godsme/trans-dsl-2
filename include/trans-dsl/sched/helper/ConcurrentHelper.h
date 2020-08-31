//
// Created by Darwin Yuan on 2020/6/13.
//

#ifndef TRANS_DSL_2_CONCURRENTHELPER_H
#define TRANS_DSL_2_CONCURRENTHELPER_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/utils/ActionTuple.h>
#include <trans-dsl/sched/action/SchedConcurrent.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <cub/utils/RepeatMacros.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/domain/TransListenerObservedAids.h>

TSL_NS_BEGIN

struct SchedAction;

namespace details {

   template<typename ... T_ACTIONS>
   class Concurrent final  {
      static constexpr size_t Num_Of_Actions = sizeof...(T_ACTIONS);
      static_assert(Num_Of_Actions >= 2, "# of concurrent actions should be at least 2");

      template<typename ... Tss>
      struct Base  {
         using ThreadActionCreator = ThreadCreator_t<Tss...>;
      protected:
         ActionTuple<Tss...> actions_;
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

         OVERRIDE(get(SeqInt seq) -> SchedAction*) {
            return Base::actions_.get(seq);
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
