//
// Created by Darwin Yuan on 2020/7/8.
//

#ifndef TRANS_DSL_2_TYPELIST_H
#define TRANS_DSL_2_TYPELIST_H

#include <cub/cub_ns.h>

CUB_NS_BEGIN

template<typename ... Ts>
struct TypeList;

template<typename H, typename ... Ts>
struct TypeList<H, Ts...> {
   using Head = H;
   using Tail = TypeList<Ts...>;
   using type = void;
};

template<>
struct TypeList<> {
   using Tail = TypeList<>;
   using type = void;
};

///////////////////////////////////////////////
template<
   template<typename ...> typename RESULT,
   typename ... Ts>
struct GenericTypeList;

template<
   template<typename ...> typename RESULT,
   typename H,
   typename ... Ts>
struct GenericTypeList<RESULT, H, Ts...> {
   using Head = H;
   using Tail = GenericTypeList<RESULT, Ts...>;

   using type = RESULT<H, Ts...>;
};

template<template<typename ...> typename RESULT>
struct GenericTypeList<RESULT> {
   using Tail = GenericTypeList<RESULT>;
   using type = RESULT<>;
};

#define __EMPTY_OUTPUT_TYPE_LIST___
#define __TYPE_LIST_APPEND(first, second) first, second

CUB_NS_END

#endif //TRANS_DSL_2_TYPELIST_H
