//
// Created by Darwin Yuan on 2020/7/8.
//

#ifndef TRANS_DSL_2_TYPELISTSPLIT_H
#define TRANS_DSL_2_TYPELISTSPLIT_H

#include <cub/cub_ns.h>
#include <cstddef>
#include <cub/type-list/TypeList.h>

CUB_NS_BEGIN

namespace details {
   template<
      size_t N,
      typename IN,
      typename ... OUT>
   struct Split {
      template<template <typename ...> typename RESULT_1, template <typename ...> typename RESULT_2>
      using output = typename Split<
         N - 1,
         typename IN::Tail,
         __TYPE_LIST_APPEND(OUT..., typename IN::Head)
      >::template output<RESULT_1, RESULT_2>;
   };

   template<
      typename IN,
      typename ... OUT>
   struct Split<0, IN, OUT...> {
      template<template <typename ...> typename RESULT_1, template <typename ...> typename RESULT_2>
      struct output {
         using first  = RESULT_1<OUT...>;
         using second = typename IN::template output<RESULT_2>;
      };
   };

   template<typename ...>
   struct __never_used_dummy__ {};
}

//////////////////////////////////////////////////////////////////////
template<
   size_t N,
   template<typename ...> typename RESULT_1,
   template<typename ...> typename RESULT_2,
   typename ... IN>
using Split_t =
   typename details::Split<
      N,
      TypeList<IN...>
      __EMPTY_OUTPUT_TYPE_LIST___
   >::template output<RESULT_1, RESULT_2>;

//////////////////////////////////////////////////////////////////////
template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using take_t =
   typename details::Split<
      N,
      TypeList<IN...>
      __EMPTY_OUTPUT_TYPE_LIST___
   >::template output<RESULT, details::__never_used_dummy__>::first;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using take_tt = typename take_t<N, RESULT, IN...>::type;

//////////////////////////////////////////////////////////////////////
namespace details {
   template<
      size_t N,
      template<typename ...> typename RESULT,
      typename ... IN>
   struct DropRight {
      static_assert(N >= sizeof...(IN), "N is greater than the size of type list");
      using type = take_t<sizeof...(IN) - N, RESULT, IN...>;
   };
}

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using dropRight_t = typename details::DropRight<N, RESULT, IN...>::type;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using dropRight_tt = typename dropRight_t<N, RESULT, IN...>::type;

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTSPLIT_H
