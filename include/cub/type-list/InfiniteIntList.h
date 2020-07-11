//
// Created by Darwin Yuan on 2020/7/11.
//

#ifndef TRANS_DSL_2_INFINITEINTLIST_H
#define TRANS_DSL_2_INFINITEINTLIST_H

#include <cstddef>
#include <cub/cub_ns.h>

CUB_NS_BEGIN

   struct __ValueKind{};
   struct __TypeKind{};

   template <auto V>
   auto DeductionKind() -> __ValueKind;

   template <typename T>
   auto DeductionKind() -> __TypeKind;

template <typename T, T N, T STEP = 1>
struct InfiniteValueList {
   constexpr static T Head = N;
   using Tail = InfiniteValueList<T, N + STEP, STEP>;
};

template <auto V>
struct RepeatValueList {
   constexpr static auto Head = V;
   using Tail = RepeatValueList<V>;
};

template <typename T>
struct RepeatTypeList {
   using Head = T;
   using Tail = RepeatTypeList<T>;
};

CUB_NS_END

#endif //TRANS_DSL_2_INFINITEINTLIST_H
