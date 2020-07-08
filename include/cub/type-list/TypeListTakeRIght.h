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
      template<typename ...> typename RESULT,
      typename ... Ts>
   struct Drop;

   template<
      size_t N,
      template<typename ...> typename RESULT,
      typename H,
      typename ... Ts>
   struct Drop<N, RESULT, H, Ts...> {
      using type = typename Drop<N-1, RESULT, Ts...>::type;
   };

   template<
      template<typename ...> typename RESULT,
      typename H,
      typename ... Ts>
   struct Drop<0, RESULT, H, Ts...> {
      using type = RESULT<H, Ts...>;
   };

   template<
      size_t N,
      template<typename ...> typename RESULT,
      typename ... Ts>
   struct TakeRight {
      static_assert(N <= sizeof...(Ts), "N is bigger than sizeof type list");
      using type = typename Drop<sizeof...(Ts) - N, RESULT, Ts...>::type;
   };
}

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using Drop_t = typename details::Drop<N, RESULT, Ts...>::type;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using Drop_tt = typename Drop_t<N, RESULT, Ts...>::type;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using TakeRight_t = typename details::TakeRight<N, RESULT, Ts...>::type;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... Ts>
using TakeRight_tt = typename TakeRight_t<N, RESULT, Ts...>::type;

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTTAKERIGHT_H
