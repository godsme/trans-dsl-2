//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_SEQUENTIALHELPER_H
#define TRANS_DSL_2_SEQUENTIALHELPER_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/action/SchedSequential.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/sched/helper/VolatileSeq.h>
#include <trans-dsl/utils/ThreadActionTrait.h>

TSL_NS_BEGIN

struct SchedAction;

namespace details {
   template<typename T, typename =  void>
   constexpr static size_t TotalSeqActions = 1;

   template<typename T>
   constexpr static size_t TotalSeqActions<T, std::enable_if_t<std::is_base_of_v<SchedSequential, T>>>
      = T::totalActions;

   template<CONCEPT(SchedActionConcept) ... T_ACTIONS>
   class Sequential final {
      enum { Num_Of_Actions = sizeof...(T_ACTIONS) };
      static_assert(Num_Of_Actions >= 2, "__sequential must contain at least 2 actions");
      static_assert(Num_Of_Actions <= 50, "too many actions in a __sequential");

      ///////////////////////////////////////////////////////////////////////////////////////////
      template<size_t N, typename T, typename=void>
      struct TypeTrait {
         using type = T;
      };

      template<size_t N, typename T>
      struct TypeTrait<N, T, std::enable_if_t<std::is_base_of_v<SchedSequential, T>>> {
         using type = typename T::template ActionType<N>;
      };

      ///////////////////////////////////////////////////////////////////////////////////////////
      template <size_t N, typename ... Ts>
      struct Extractor;

      ///////////////////////////////////////////////////////////////////////////////////////////
      template<size_t N, bool Value, typename ... Ts>
      struct Calc;

      template<size_t N, typename H, typename ... Ts>
      struct Calc<N, true, H, Ts...> {
         using type =  typename TypeTrait<N, H, void>::type;
      };

      template<size_t N, typename H, typename ... Ts>
      struct Calc<N, false, H, Ts...> {
         using type =  typename Extractor<N - TotalSeqActions<H>, Ts...>::type;
      };

      ///////////////////////////////////////////////////////////////////////////////////////////
      template <size_t N, typename T, typename ... Ts>
      struct Extractor<N, T, Ts...> {
         using type =  typename Calc<N, N < TotalSeqActions<T>, T, Ts...>::type;
      };

      ///////////////////////////////////////////////////////////////////////////////////////////
      template <typename ... Ts>
      struct Extract {
         template<size_t N>
         using type = typename Extractor<N, Ts...>::type;
      };

      ///////////////////////////////////////////////////////////////////////////////////////////
      template<size_t N, size_t M, template<typename ...> typename USER, typename EXTRACTOR, typename ... Ts>
      struct Comb {
         using elem = typename EXTRACTOR::template type<M>;
         using type = typename Comb<N-1, M+1, USER, EXTRACTOR, Ts..., elem>::type;
      };

      template<size_t M, template<typename ...> typename USER, typename EXTRACTOR, typename ... Ts>
      struct Comb<0, M, USER, EXTRACTOR, Ts...> {
         using type = USER<Ts..., typename EXTRACTOR::template type<M>>;
      };

      ///////////////////////////////////////////////////////////////////////////////////////////
      template<typename ... Ts>
      using AllActionsSeq = VolatileSeq<SchedAction, Ts...>;
      constexpr static size_t totalNumOfActions = (TotalSeqActions<T_ACTIONS> + ... );
      using Base = typename Comb<totalNumOfActions-1, 0, AllActionsSeq, Extract<T_ACTIONS...>>::type;

   public:
      struct Inner final : SchedSequential, private Base {
         // for fork thread creator
         using ThreadActionCreator = ThreadCreator_t<T_ACTIONS...>;

         // for seq elision
         constexpr static size_t totalActions = (TotalSeqActions<T_ACTIONS> + ... );

         template<size_t N>
         using ActionType = typename Extractor<N, T_ACTIONS...>::type;

      private:
         OVERRIDE(getNumOfActions()->SeqInt) { return Num_Of_Actions; }
         OVERRIDE(getNext(SeqInt seq) -> SchedAction*) {
            return Base::get(seq);
         }
      };
   };

   template<typename ... Ts>
   using Sequential_  = typename Sequential<Ts...>::Inner;
}

#define __sequential(...) TSL_NS::details::Sequential_<__VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_SEQUENTIALHELPER_H
