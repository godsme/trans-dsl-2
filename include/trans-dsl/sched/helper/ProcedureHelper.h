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

   template<CONCEPT(SchedActionConcept) ... T_ACTIONS>
   using FinalAction = typename AutoSeq<FinallySignature>::template Inner<T_ACTIONS...>;

   template<typename T>
   DEF_CONCEPT(FinallyConcept, std::is_base_of_v<FinallySignature, T>);

   template<typename ... Ts>
   class ProcedureTrait final {
      struct MainActionSignature {};

      template<typename ... T_ACTIONS>
      using MainActionTrait = typename AutoSeq<MainActionSignature>::template Inner<T_ACTIONS...>;

      template<typename ... Tss>
      struct FinalTrait;

      template<CONCEPT(FinallyConcept) T>
      struct FinalTrait<T> {
         CONCEPT_ASSERT(FinallyConcept<T>);
         using type = typename T::type;
      };

   public:
      using type = Split_t<sizeof...(Ts) - 2, MainActionTrait, FinalTrait, Ts...>;
   };

   template<bool V_IS_PROTECTED, typename ... T_ACTIONS>
   struct Procedure final : SchedProcedure {
   private:
      using MainAction = typename ProcedureTrait<T_ACTIONS...>::type::first::type;
      using FinalAction = typename ProcedureTrait<T_ACTIONS...>::type::second::type;

      using ThreadActionCreator = ThreadCreator_t<MainAction, FinalAction>;
   private:
      OVERRIDE(getAction()->SchedAction *) {
         return new(cache) MainAction;
      }

      OVERRIDE(getFinalAction()->SchedAction *) {
         return new(cache) FinalAction;
      }

      OVERRIDE(isProtected() const -> bool) {
         return V_IS_PROTECTED;
      }

   private:
      enum : size_t {
         alignment = std::max(alignof(MainAction), alignof(FinalAction)),
         size = std::max(sizeof(MainAction), sizeof(FinalAction))
      };

      alignas(alignment) unsigned char cache[size];
   };
}

#define __internal_PrOCeDuRe(prot, ...) TSL_NS::details::Procedure<prot, __VA_ARGS__>

////////////////////////////////////////////////////////////////////////////////////////
#define __procedure(...) __internal_PrOCeDuRe(false, __VA_ARGS__)
#define __prot_procedure(...) __internal_PrOCeDuRe(true, __VA_ARGS__)

#define __finally(...)   TSL_NS::details::FinalAction<__VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_PROCEDUREHELPER_H
