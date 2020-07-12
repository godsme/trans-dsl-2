//
// Created by Darwin Yuan on 2020/7/11.
//

#ifndef TRANS_DSL_2_TYPELISTVALUEWRAPPER_H
#define TRANS_DSL_2_TYPELISTVALUEWRAPPER_H

#include <cub/cub_ns.h>
#include <type_traits>
#include <cub/type-list/ValueListSignature.h>

CUB_NS_BEGIN

struct ValueWrapperSignature {};

template <auto VALUE>
struct V : ValueWrapperSignature {
   constexpr static auto value = VALUE;
};

template<typename T, typename = void>
struct ValueTrait;

template<typename T>
struct ValueTrait<T, std::enable_if_t<std::is_base_of_v<ValueWrapperSignature, T>>> {
   constexpr static auto value = T::value;
};

template <typename LIST, typename = void>
struct ListWrapper {
   using Head = typename LIST::Head;
   using Tail = typename LIST::Tail;
};

template<typename LIST>
struct ListWrapper<LIST, std::enable_if_t<std::is_base_of_v<ValueListSignature, LIST>>> {
   using Head = V<LIST::Head>;
   using Tail = ListWrapper<typename LIST::Tail>;
};

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTVALUEWRAPPER_H
