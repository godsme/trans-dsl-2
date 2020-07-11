//
// Created by Darwin Yuan on 2020/7/10.
//

#ifndef TRANS_DSL_2_TYPELISTACCUMULATOR_H
#define TRANS_DSL_2_TYPELISTACCUMULATOR_H

#include <cub/cub_ns.h>

CUB_NS_BEGIN

template<typename ... Ts1>
struct Accumulator {
   template<typename ... Ts2>
   using type = Accumulator<Ts1..., Ts2...>;

   template<template<typename ...> typename RESULT>
   using output = RESULT<Ts1...>;
};

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTACCUMULATOR_H
