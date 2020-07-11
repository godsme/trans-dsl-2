//
// Created by Darwin Yuan on 2020/7/8.
//

#ifndef TRANS_DSL_2_TYPELIST_H
#define TRANS_DSL_2_TYPELIST_H

#include <cub/cub_ns.h>

CUB_NS_BEGIN

template<typename ... Ts>
struct TypeList {
   template<template <typename ...> typename RESULT>
   using output = RESULT<>;

   template<typename ... Ts2>
   using concat = TypeList<Ts2...>;
};

template<typename H, typename ... Ts>
struct TypeList<H, Ts...> {
   using Head = H;
   using Tail = TypeList<Ts...>;

   template<typename ... Ts2>
   using concat = TypeList<H, Ts..., Ts2...>;

   template<template <typename ...> typename RESULT>
   using output = RESULT<H, Ts...>;
};

#define __EMPTY_OUTPUT_TYPE_LIST___
#define __TYPE_LIST_APPEND(first, second) first, second

CUB_NS_END

#endif //TRANS_DSL_2_TYPELIST_H
