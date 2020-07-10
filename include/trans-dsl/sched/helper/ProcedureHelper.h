//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_PROCEDUREHELPER_H
#define TRANS_DSL_2_PROCEDUREHELPER_H

#include <trans-dsl/sched/action/SchedProcedure.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/helper/SequentialHelper.h>
#include <trans-dsl/sched/helper/AutoSeqHelper.h>
#include <cub/type-list/TypeListSplit.h>

TSL_NS_BEGIN

namespace details {
   struct FinallySignature {};

   template<bool V_IS_RECOVER, typename T_FINAL>
   struct FinalAction : FinallySignature {
      constexpr static bool isRecover = V_IS_RECOVER;
      using type = T_FINAL;
   };

   template<typename T>
   DEF_CONCEPT(FinallyConcept, std::is_base_of_v<FinallySignature, T>);

   template<typename ... Tss>
   struct FinalTrait;

   template<CONCEPT(FinallyConcept) T>
   struct FinalTrait<T> : T {
      CONCEPT_ASSERT(FinallyConcept<T>);
   };

   template<typename ... T_ACTIONS>
   class Procedure final  {
      static_assert(sizeof...(T_ACTIONS) > 1, "__procedure should have at 1 action and 1 final action");

      using FakeType = CUB_NS::Split_t<sizeof...(T_ACTIONS) - 1, AutoSeq<>::template Inner, FinalTrait, T_ACTIONS...>;

   public:
      template<TransListenerObservedAids const& AIDs>
      struct ActionRealType : SchedProcedure {
      private:
         using MainAction = ActionRealTypeTraits_t<AIDs, typename FakeType::first::type>;
         using FinalType = typename FakeType::second;
         using FinalAction = ActionRealTypeTraits_t<AIDs, typename FinalType::type>;

         static_assert(SchedActionConcept<MainAction>);
         static_assert(SchedActionConcept<FinalAction>);

      public:
         using ThreadActionCreator = ThreadCreator_t<MainAction, FinalAction>;

      private:
         OVERRIDE(getAction()->SchedAction *) {
            return new(cache) MainAction;
         }

         OVERRIDE(getFinalAction()->SchedAction *) {
            return new(cache) FinalAction;
         }

         OVERRIDE(isProtected() const -> bool) {
            return FinalType::isRecover;
         }

      private:
         enum : size_t {
            alignment = std::max(alignof(MainAction), alignof(FinalAction)),
            size = std::max(sizeof(MainAction), sizeof(FinalAction))
         };

         alignas(alignment) unsigned char cache[size];
      };
   };

   template<typename ... Ts>
   using Procedure_t = typename Procedure<Ts...>::template ActionRealType<EmptyAids>;
}

////////////////////////////////////////////////////////////////////////////////////////
#define __procedure(...) TSL_NS::details::Procedure<__VA_ARGS__>
#define __def_procedure(...) TSL_NS::details::Procedure_t<__VA_ARGS__>

#define __finally(...)   \
 TSL_NS::details::FinalAction<false, TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>>
#define __recover(...)  \
 TSL_NS::details::FinalAction<true,  TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>>

TSL_NS_END

#endif //TRANS_DSL_2_PROCEDUREHELPER_H
