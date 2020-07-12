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
   using __Secrete_OutputAllTypesTo = RESULT<Ts...>;
};

class FlattenSeq final {
   template<typename ACC, typename T, typename = void>
   struct Seq_ {
      using type = typename ACC::template type<T>;
   };

   template<typename ACC, typename T>
   struct Seq_<ACC, T, std::enable_if_t < std::is_base_of_v < CUB_NS::FlattenableSignature, T>>> {
      using type = typename T::template __Secrete_OutputAllTypesTo<ACC::template type>;
   };

   template<typename ACC, typename T>
   using Seq = Seq_<ACC, T, void>;

public:
   template<typename IN>
   using type = type_list::FoldLInit_t<IN, Seq, Accumulator<>>;
};

namespace type_list {
   template <typename IN>
   using Flatten_t = typename FlattenSeq::template type<IN>;
}

template <template<typename ...> typename RESULT, typename ... Ts>
using Flatten_t = typename type_list::Flatten_t<TypeList<Ts...>>::template output<RESULT>;

CUB_NS_END

#endif //TRANS_DSL_2_FLATTENABLE_H
