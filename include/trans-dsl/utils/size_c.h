//
// Created by Darwin Yuan on 2020/8/31.
//

#ifndef TRANS_DSL_2_SIZE_C_H
#define TRANS_DSL_2_SIZE_C_H

#include <trans-dsl/tsl_ns.h>
#include <type_traits>
#include <trans-dsl/utils/integral_c.h>

TSL_NS_BEGIN

template <std::size_t V>
using size_c_t = integral_c<std::size_t, V>;

template <std::size_t V>
constexpr size_c_t<V> size_c{};

TSL_NS_END

#endif //TRANS_DSL_2_SIZE_C_H
