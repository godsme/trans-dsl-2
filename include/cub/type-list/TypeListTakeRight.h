//
// Created by Darwin Yuan on 2020/7/8.
//

#ifndef TRANS_DSL_2_TYPELISTTAKERIGHT_H
#define TRANS_DSL_2_TYPELISTTAKERIGHT_H

#include <cub/cub_ns.h>
#include <cub/type-list/TypeList.h>
#include <type_traits>

CUB_NS_BEGIN

namespace details {
   template<
      size_t N,
      typename ... Ts>
   struct Drop;

   template<
      size_t N,
      typename H,
      typename ... Ts>
   struct Drop<N, H, Ts...> {
      template<template<typename ...> typename RESULT>
      using output = typename Drop<N-1, Ts...>::template output<RESULT>;
   };

   template<
      typename H,
      typename ... Ts>
   struct Drop<0, H, Ts...> {
      template<template<typename ...> typename RESULT>
      using output = RESULT<H, Ts...>;
   };

   template<>
   struct Drop<0> {
      template<template<typename ...> typename RESULT>
      using output = RESULT<>;
   };

   template<
      size_t N,
      typename ... Ts>
   struct TakeRight {
      static_assert(N <= sizeof...(Ts), "N is bigger than sizeof type list");
      template<template<typename ...> typename RESULT>
      using output = typename Drop<sizeof...(Ts) - N, Ts...>::template output<RESULT>;
   };

   template<typename ... Ts>
   struct Head;

   template<typename H, typename ... Ts>
   struct Head<H, Ts...> {
      using type = H;
   };
}

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using Drop_t = typename details::Drop<N, Ts...>::template output<RESULT>;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using Drop_tt = typename Drop_t<N, RESULT, Ts...>::type;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using TakeRight_t = typename details::TakeRight<N, Ts...>::template output<RESULT>;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using TakeRight_tt = typename TakeRight_t<N, RESULT, Ts...>::type;

template<
   size_t N,
   typename ... Ts>
using Elem_t = Drop_tt<N, details::Head, Ts...>;


CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTTAKERIGHT_H
