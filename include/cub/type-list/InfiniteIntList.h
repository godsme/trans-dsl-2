//
// Created by Darwin Yuan on 2020/7/11.
//

#ifndef TRANS_DSL_2_INFINITEINTLIST_H
#define TRANS_DSL_2_INFINITEINTLIST_H

#include <cstddef>
#include <cub/cub_ns.h>

CUB_NS_BEGIN

template <typename T, T N, T STEP = 1>
struct InfiniteValueList {
   using type = T;
   constexpr static T Head = N;
   using Tail = InfiniteValueList<T, N + STEP, STEP>;
};

template <typename T, T V>
struct RepeatValueList {
   using type = T;
   constexpr static T Head = V;
   using Tail = RepeatValueList<T, V>;
};

template <typename T>
struct RepeatTypeList {
   using Head = T;
   using Tail = RepeatTypeList<T>;
};

CUB_NS_END

#endif //TRANS_DSL_2_INFINITEINTLIST_H
