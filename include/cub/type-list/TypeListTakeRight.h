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
      size_t    N,
      typename  TYPE_LIST,
      typename = void>
   struct Drop;

   template<typename  TYPE_LIST>
   struct Drop<0, TYPE_LIST> {
      template<template<typename ...> typename RESULT>
      using output = typename TYPE_LIST::template output<RESULT>;
   };

   template<
      size_t N,
      typename  TYPE_LIST>
   struct Drop<N, TYPE_LIST, std::void_t<typename TYPE_LIST::Head>> {
      template<template<typename ...> typename RESULT>
      using output = typename Drop<N-1, typename TYPE_LIST::Tail>::template output<RESULT>;
   };

   template<
      size_t N,
      typename  TYPE_LIST>
   struct TakeRight {
      static_assert(N <= TYPE_LIST::size, "N is bigger than sizeof type list");
      template<template<typename ...> typename RESULT>
      using output = typename Drop<TYPE_LIST::size - N, TYPE_LIST>::template output<RESULT>;
   };

   template<typename ... Ts>
   struct Head;

   template<typename H, typename ... Ts>
   struct Head<H, Ts...> {
      using type = H;
   };
}

namespace type_list {
   template<
      size_t N,
      template<typename ...> typename RESULT,
      typename TYPE_LIST>
   using Drop_t = typename details::Drop<N, TYPE_LIST>::template output<RESULT>;

   template<
      size_t N,
      template<typename ...> typename RESULT,
      typename TYPE_LIST>
   using Drop_tt = typename Drop_t<N, RESULT, TYPE_LIST>::type;

   template<
      size_t N,
      template<typename ...> typename RESULT,
      typename TYPE_LIST>
   using TakeRight_t = typename details::TakeRight<N, TYPE_LIST>::template output<RESULT>;

   template<
      size_t N,
      typename TYPE_LIST>
   using Elem_t = Drop_tt<N, details::Head, TYPE_LIST>;
}

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using Drop_t = type_list::Drop_t<N, RESULT, TypeList<Ts...>>;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using Drop_tt = typename Drop_t<N, RESULT, Ts...>::type;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using TakeRight_t = type_list::TakeRight_t<N, RESULT, TypeList<Ts...>>;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using TakeRight_tt = typename TakeRight_t<N, RESULT, Ts...>::type;

template<
   size_t N,
   typename ... Ts>
using Elem_t = type_list::Elem_t<N, TypeList<Ts...>>;


CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTTAKERIGHT_H
