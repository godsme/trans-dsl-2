//
// Created by Darwin Yuan on 2020/7/11.
//

#ifndef TRANS_DSL_2_INFINITEINTLIST_H
#define TRANS_DSL_2_INFINITEINTLIST_H

#include <cub/type-list/TypeListValueWrapper.h>
#include <cub/type-list/ValueListSignature.h>

CUB_NS_BEGIN

template <typename T, T N, T STEP = 1>
struct InfiniteValueList : ValueListSignature {
   constexpr static T Head = N;
   using Tail = InfiniteValueList<T, N + STEP, STEP>;
};

template <auto V>
struct RepeatValueList : ValueListSignature {
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
