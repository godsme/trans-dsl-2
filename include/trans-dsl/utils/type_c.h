//
// Created by Darwin Yuan on 2020/8/31.
//

#ifndef TRANS_DSL_2_TYPE_C_H
#define TRANS_DSL_2_TYPE_C_H

#include <trans-dsl/tsl_ns.h>
#include <type_traits>
#include <trans-dsl/utils/integral_c.h>

TSL_NS_BEGIN

template <typename T>
struct type_c_t {
   using type = T;
};

template <typename U, typename V>
inline constexpr auto operator==(type_c_t<U> const&, type_c_t<V> const&) noexcept {
   return bool_c<std::is_same_v<U, V>>;
}

template <typename U, typename V>
inline constexpr auto operator!=(type_c_t<U> const&, type_c_t<V> const&) noexcept {
   return bool_c<!std::is_same_v<U, V>>;
}

template <typename T>
constexpr type_c_t<T> type_c{};

TSL_NS_END

#endif //TRANS_DSL_2_TYPE_C_H
