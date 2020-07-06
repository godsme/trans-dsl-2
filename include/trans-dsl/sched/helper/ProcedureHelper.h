//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_PROCEDUREHELPER_H
#define TRANS_DSL_2_PROCEDUREHELPER_H

#include <trans-dsl/sched/action/SchedProcedure.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/helper/SequentialHelper.h>
#include <trans-dsl/utils/TypeListSpliter.h>
#include <trans-dsl/sched/helper/AutoSeqHelper.h>
TSL_NS_BEGIN

namespace details {
   struct FinallySignature {};

   template<bool V_IS_RECOVER, CONCEPT(SchedActionConcept) ... T_ACTIONS>
   struct FinalAction : public AutoSeq<FinallySignature>::template Inner<T_ACTIONS...> {
      constexpr static bool isRecover = V_IS_RECOVER;
   };

   template<typename T>
   DEF_CONCEPT(FinallyConcept, std::is_base_of_v<FinallySignature, T>);

   template<typename ... Tss>
   struct FinalTrait;

   template<CONCEPT(FinallyConcept) T>
   struct FinalTrait<T> {
      CONCEPT_ASSERT(FinallyConcept<T>);
      constexpr static bool isRecover = T::isRecover;
      using type = typename T::type;
   };

   template<typename ... T_ACTIONS>
   struct Procedure final : SchedProcedure {
      static_assert(sizeof...(T_ACTIONS) > 1, "__procedure should have at 1 action and 1 final action");
   private:
      template<typename ... Ts>
      class Trait final {
         struct MainActionSignature {};

         template<typename ... Tss>
         using MainActionTrait = typename AutoSeq<MainActionSignature>::template Inner<Tss...>;

         using type = Split_t<sizeof...(Ts) - 1, MainActionTrait, FinalTrait, Ts...>;

      public:
         using MainType = typename type::first::type;
         using FinalType = typename type::second;
      };

      using MainAction  = typename Trait<T_ACTIONS...>::MainType;
      using FinalAction = typename Trait<T_ACTIONS...>::FinalType::type;

      constexpr static bool isRecover = Trait<T_ACTIONS...>::FinalType::isRecover;

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
         return isRecover;
      }

   private:
      enum : size_t {
         alignment = std::max(alignof(MainAction), alignof(FinalAction)),
         size = std::max(sizeof(MainAction), sizeof(FinalAction))
      };

      alignas(alignment) unsigned char cache[size];
   };
}

////////////////////////////////////////////////////////////////////////////////////////
#define __procedure(...) TSL_NS::details::Procedure<__VA_ARGS__>

#define __finally(...)   TSL_NS::details::FinalAction<false, __VA_ARGS__>
#define __recover(...)   TSL_NS::details::FinalAction<true, __VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_PROCEDUREHELPER_H
