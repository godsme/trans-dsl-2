//
// Created by Darwin Yuan on 2020/7/10.
//

#ifndef TRANS_DSL_2_FLATTENABLE_H
#define TRANS_DSL_2_FLATTENABLE_H

#include <cub/cub_ns.h>
#include <cub/type-list/TypeListAccumulator.h>
#include <cub/type-list/TypeListFold.h>
#include <type_traits>

CUB_NS_BEGIN

struct FlattenableSignature {};

template<typename ... Ts>
struct Flattenable : FlattenableSignature {
   template<template<typename ...> typename RESULT>
   using OutputAllTypesTo = RESULT<Ts...>;
};

class FlattenSeq final {
   template<typename COMPOSER, typename T, typename = void>
   struct Seq_ {
      using type = typename COMPOSER::template type<T>;
   };

   template<typename COMPOSER, typename T>
   struct Seq_<COMPOSER, T, std::enable_if_t < std::is_base_of_v < CUB_NS::FlattenableSignature, T>>> {
      using type = typename T::template OutputAllTypesTo<COMPOSER::template type>;
   };

   template<typename COMPOSER, typename T>
   using Seq = Seq_<COMPOSER, T, void>;

public:
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = typename FoldL_Init_t<Seq, Accumulator<>, Ts...>::template output<RESULT>;
};

template <template<typename ...> typename RESULT, typename ... Ts>
using Flatten_t = typename FlattenSeq::template type<RESULT, Ts...>;

CUB_NS_END

#endif //TRANS_DSL_2_FLATTENABLE_H
