//
// Created by Darwin Yuan on 2020/8/31.
//

#ifndef TRANS_DSL_2_INTEGRAL_C_H
#define TRANS_DSL_2_INTEGRAL_C_H

#include <trans-dsl/tsl_ns.h>
#include <type_traits>

TSL_NS_BEGIN

template<typename T, T V>
struct integral_c {
   constexpr static auto value() { return V; }
   constexpr operator T() const { return V; }
};

template<bool V>
using bool_c_t = integral_c<bool, V>;

template<bool V>
constexpr bool_c_t<V> bool_c{};

using true_type  = integral_c<bool, true>;
using false_type = integral_c<bool, false>;


template<typename T>
struct Is_Integral_Const : false_type {};

template<typename T, T V>
struct Is_Integral_Const<integral_c<T, V>> : true_type {};

template<typename T>
constexpr auto Is_Integral_Const_V = Is_Integral_Const<T>::value();

template<typename T>
constexpr auto Is_True_V = std::is_same_v<true_type, T>;

template<typename T>
constexpr bool is_integral_const(T const&) {
   return Is_Integral_Const_V<std::decay_t<T>>;
}

constexpr bool_c_t<true> true_c{};
constexpr bool_c_t<false> false_c{};

template<bool X, bool Y>
auto operator||(bool_c_t<X>, bool_c_t<Y>) {
   return bool_c<X || Y>;
}

template<bool X, bool Y>
auto operator&&(bool_c_t<X>, bool_c_t<Y>) {
   return bool_c<X && Y>;
}

template<typename T, T V1, T V2>
inline constexpr auto operator==(integral_c<T, V1> const&, integral_c<T, V2> const&) {
   return bool_c<V1 == V2>;
}

template<typename T, T V1, T V2>
inline constexpr auto operator!=(integral_c<T, V1> const&, integral_c<T, V2> const&) {
   return bool_c<V1 != V2>;
}

template<typename T, T V1, T V2>
inline constexpr auto operator<(integral_c<T, V1> const&, integral_c<T, V2> const&) {
   return bool_c<V1 < V2>;
}

template<typename T, T V1, T V2>
inline constexpr auto operator<=(integral_c<T, V1> const&, integral_c<T, V2> const&) {
   return bool_c<V1 <= V2>;
}

template<typename T, T V1, T V2>
inline constexpr auto operator>(integral_c<T, V1> const&, integral_c<T, V2> const&) {
   return bool_c<(V1 > V2)>;
}

template<typename T, T V1, T V2>
inline constexpr auto operator>=(integral_c<T, V1> const&, integral_c<T, V2> const&) {
   return bool_c<(V1 >= V2)>;
}

TSL_NS_END

#endif //TRANS_DSL_2_INTEGRAL_C_H
