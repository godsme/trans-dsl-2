//
// Created by Darwin Yuan on 2020/7/6.
//

#ifndef TRANS_DSL_2_INLINESEQHELPER_H
#define TRANS_DSL_2_INLINESEQHELPER_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/action/SchedSequential.h>

TSL_NS_BEGIN

struct SchedAction;

namespace details::inline_seq {

   template<typename T, typename =  void>
   constexpr static size_t TotalSeqActions = 1;

   template<typename T>
   constexpr static size_t TotalSeqActions<T, std::enable_if_t<std::is_base_of_v<SchedSequential, T>>>
   = T::totalActions;

   ///////////////////////////////////////////////////////////////////////////////////////////
   template<size_t N, typename T, typename=void>
   struct TypeTrait {
      using type = T;
   };

   template<size_t N, typename T>
   struct TypeTrait<N, T, std::enable_if_t < std::is_base_of_v < SchedSequential, T>>> {
      using type = typename T::template ActionType<N>;
   };

   ///////////////////////////////////////////////////////////////////////////////////////////
   template<size_t N, typename ... Ts>
   struct Extractor;

   ///////////////////////////////////////////////////////////////////////////////////////////
   template<size_t N, bool Value, typename ... Ts>
   struct Calc;

   template<size_t N, typename H, typename ... Ts>
   struct Calc<N, true, H, Ts...> {
      using type = typename TypeTrait<N, H, void>::type;
   };

   template<size_t N, typename H, typename ... Ts>
   struct Calc<N, false, H, Ts...> {
      using type = typename Extractor<N - TotalSeqActions<H>, Ts...>::type;
   };

   ///////////////////////////////////////////////////////////////////////////////////////////
   template<size_t N, typename T, typename ... Ts>
   struct Extractor<N, T, Ts...> {
      using type = typename Calc<N, N < TotalSeqActions<T>, T, Ts...>::type;
   };

   ///////////////////////////////////////////////////////////////////////////////////////////
   template<typename ... Ts>
   struct Extract {
      template<size_t N>
      using type = typename Extractor<N, Ts...>::type;
   };

   ///////////////////////////////////////////////////////////////////////////////////////////
   template<size_t N, size_t M, template<typename ...> typename USER, typename EXTRACTOR, typename ... Ts>
   struct Inlined {
      using elem = typename EXTRACTOR::template type<M>;
      using type = typename Inlined<N - 1, M + 1, USER, EXTRACTOR, Ts..., elem>::type;
   };

   template<size_t M, template<typename ...> typename USER, typename EXTRACTOR, typename ... Ts>
   struct Inlined<0, M, USER, EXTRACTOR, Ts...> {
      using type = USER<Ts..., typename EXTRACTOR::template type<M>>;
   };

   template<template<typename ...> typename USER, typename ... Ts>
   struct Inlined_t {
      constexpr static size_t totalNumOfActions = (inline_seq::TotalSeqActions<Ts> + ... );
      using type =  typename inline_seq::Inlined<totalNumOfActions - 1, 0, USER, inline_seq::Extract<Ts...>>::type;
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_INLINESEQHELPER_H
