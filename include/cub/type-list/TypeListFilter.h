//
// Created by Darwin Yuan on 2020/7/10.
//

#ifndef TRANS_DSL_2_TYPELISTFILTER_H
#define TRANS_DSL_2_TYPELISTFILTER_H

#include <cub/cub_ns.h>
#include <cub/type-list/TypeList.h>
#include <type_traits>

CUB_NS_BEGIN

namespace details {

   template<typename IN, template<typename> typename PRED, typename = void, typename  ... OUT>
   struct Filter {
      template<template<typename ...> typename RESULT>
      using output = RESULT<OUT...>;
   };

   template<
      typename                    IN,
      template<typename> typename PRED,
      typename                ... OUT>
   struct Filter<IN, PRED, std::enable_if<PRED<typename IN::Head>::value>, OUT...> {
      template<template<typename ...> typename RESULT>
      using output = typename Filter<
         typename IN::Tail,
         PRED,
         void,
         __TYPE_LIST_APPEND(OUT..., typename IN::Head)
      >::template output<RESULT>;
   };

   template<
      typename                    IN,
      template<typename> typename PRED,
      typename                ... OUT>
   struct Filter<IN, PRED, std::enable_if<!PRED<typename IN::Head>::value>, OUT...> {
      template<template<typename ...> typename RESULT>
      using output = typename Filter<
         typename IN::Tail,
         PRED,
         void,
         OUT...
      >::template output<RESULT>;
   };
}

template
   < template<typename> typename     PRED
   , template<typename ...> typename RESULT
   , typename                    ... IN>
using Filter_t = typename details::Filter<TypeList<IN...>, PRED>::template output<RESULT>;

template
   < template<typename> typename     PRED
   , template<typename ...> typename RESULT
   , typename                    ... IN>
using Filter_tt = Filter_t<PRED, RESULT, IN...>;

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTFILTER_H
