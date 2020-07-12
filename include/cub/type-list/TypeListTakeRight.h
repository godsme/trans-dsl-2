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
      typename  IN,
      size_t    N,
      typename = void>
   struct Drop;

   template<typename  IN>
   struct Drop<IN, 0> {
      using type = IN;
   };

   template<
      typename  IN,
      size_t N>
   struct Drop<IN, N, std::void_t<typename IN::Head>> {
      using type = typename Drop<typename IN::Tail, N-1>::type;
   };

   template<
      typename  IN,
      size_t N>
   struct TakeRight {
      static_assert(N <= IN::size, "N is bigger than sizeof type list");
      using type = typename Drop<IN, IN::size - N>::type;
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
      typename IN,
      size_t N>
   using Drop_t = typename details::Drop<IN, N>::type;

   template<
      typename IN,
      size_t N>
   using TakeRight_t = typename details::TakeRight<IN, N>::type;

   template<
      typename IN,
      size_t N>
   using Elem_t = typename Drop_t<IN, N>::Head;
}

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using Drop_t = typename type_list::Drop_t<TypeList<Ts...>, N>::template output<RESULT>;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using Drop_tt = typename Drop_t<N, RESULT, Ts...>::type;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using TakeRight_t = typename type_list::TakeRight_t<TypeList<Ts...>, N>::template output<RESULT>;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using TakeRight_tt = typename TakeRight_t<N, RESULT, Ts...>::type;

template<
   size_t N,
   typename ... Ts>
using Elem_t = type_list::Elem_t<TypeList<Ts...>, N>;


CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTTAKERIGHT_H
