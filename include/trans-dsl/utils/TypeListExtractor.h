//
// Created by Darwin Yuan on 2020/6/28.
//

#ifndef TRANS_DSL_2_TYPELISTEXTRACTOR_H
#define TRANS_DSL_2_TYPELISTEXTRACTOR_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/SeqInt.h>
#include <cstddef>

TSL_NS_BEGIN

namespace details {
   template<
      size_t N,
      template<typename ...> typename TRAIT,
      typename ... Ts>
   struct TypeListExtractor;

   template<
      size_t N,
      template<typename ...> typename TRAIT,
      typename HEAD,
      typename ... TAIL>
   struct TypeListExtractor<N, TRAIT, HEAD, TAIL...> {
      using type = typename TypeListExtractor<SeqInt(N - 1), TRAIT, TAIL...>::type;
   };

   template<
      template<typename ...> typename TRAIT,
      typename HEAD,
      typename ... TAIL>
   struct TypeListExtractor<0, TRAIT, HEAD, TAIL...> {
      using type = TRAIT<HEAD, TAIL...>;
   };

   template<
      size_t N,
      template<typename ...> typename TRAIT,
      typename ... Ts>
    using TypeListExtractor_t = typename TypeListExtractor<N, TRAIT, Ts...>::type::type;

   //////////////////////////////////////////////////
   template<typename ...> struct HeadTraits;

   template<typename HEAD, typename ...TAIL>
   struct HeadTraits<HEAD, TAIL...> final {
      using type = HEAD;
   };

   template<template<typename ...> typename USER>
   struct TailTraits {
      template<typename ... TAIL>
      struct Inner;

      template< typename HEAD, typename ...TAIL>
      struct Inner<HEAD, TAIL...> {
         using type = USER<TAIL...>;
      };
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_TYPELISTEXTRACTOR_H
