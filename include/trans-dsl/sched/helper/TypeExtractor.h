//
// Created by Darwin Yuan on 2020/6/28.
//

#ifndef TRANS_DSL_2_TYPEEXTRACTOR_H
#define TRANS_DSL_2_TYPEEXTRACTOR_H

#include <trans-dsl/tsl_ns.h>
#include <cstddef>
#include <type_traits>

TSL_NS_BEGIN

namespace details {
   template<size_t N, template<typename, typename ...> typename TRAIT, typename ... Ts>
   struct TypeExtractor;

   template<size_t N, template<typename , typename ...> typename TRAIT, typename Head, typename ... Ts>
   struct TypeExtractor<N, TRAIT, Head, Ts...> {
      using type = typename TypeExtractor<N-1, TRAIT, Ts...>::type;
   };

   template<template<typename , typename ...> typename TRAIT, typename Head, typename ... Ts>
   struct TypeExtractor<0, TRAIT, Head, Ts...> {
      using type = typename TRAIT<Head, Ts...>::type;
   };

   template<typename H, typename ...>
   struct Head {
      using type = H;
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_TYPEEXTRACTOR_H
