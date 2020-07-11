//
// Created by Darwin Yuan on 2020/7/8.
//

#ifndef TRANS_DSL_2_TYPELISTTRANSFORM_H
#define TRANS_DSL_2_TYPELISTTRANSFORM_H

#include <cub/cub_ns.h>
#include <cub/type-list/TypeList.h>
#include <type_traits>

CUB_NS_BEGIN

namespace details {
   template<
      typename IN,
      template<typename> typename F,
      typename = void,
      typename ... OUT>
   struct Transform {
      template<template<typename ...> typename RESULT>
      using output = RESULT<OUT...>;
   };

   template<
      typename IN,
      template<typename> typename F,
      typename ... OUT>
   struct Transform<IN, F, std::void_t<typename IN::Head>, OUT...> {
      template<template<typename ...> typename RESULT>
      using output =
      typename Transform<
         typename IN::Tail,
         F,
         void,
         __TYPE_LIST_APPEND(OUT..., typename F<typename IN::Head>::type)
      >::template output<RESULT>;
   };
}

namespace type_list {
   template<
      typename   IN,
      template<typename>     typename F,
      template<typename ...> typename RESULT>
   using Transform_t =
   typename details::Transform<
      IN,
      F,
      void
      __EMPTY_OUTPUT_TYPE_LIST___
   >::template output<RESULT>;
}
///////////////////////////////////////////////////////////////////////////////////////
template<
   template<typename>     typename F,
   template<typename ...> typename RESULT,
   typename                    ... IN>
using Transform_t = type_list::Transform_t<TypeList < IN...>, F, RESULT>;

///////////////////////////////////////////////////////////////////////////////////////
template<
   template<typename>     typename F,
   template<typename ...> typename  RESULT,
   typename    ... IN>
using Transform_tt = typename Transform_t<F, RESULT, IN...>::type;

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTTRANSFORM_H
