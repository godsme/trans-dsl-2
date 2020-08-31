//
// Created by Darwin Yuan on 2020/8/31.
//

#ifndef TRANS_DSL_2_EBO_H
#define TRANS_DSL_2_EBO_H

#include <trans-dsl/tsl_ns.h>
#include <type_traits>

TSL_NS_BEGIN

namespace detail {
   template<typename T>
   constexpr bool Is_Non_Final_Class = std::is_class_v<T> && !std::is_final_v<T>;

   template<typename K, typename V,
      bool = Is_Non_Final_Class<V>,
      typename = std::enable_if_t<std::is_copy_constructible_v<V> && std::is_default_constructible_v<V>>>
   struct ebo;

   template<typename K, typename V>
   struct ebo<K, V, true> : V {
      constexpr ebo() = default;
      constexpr explicit ebo(V const &v) : V{v} {}
      constexpr ebo(ebo const &rhs) = default;

      template<typename T, typename = std::enable_if_t<!std::is_same_v<V, T>>>
      constexpr ebo(T const &rhs) : V{rhs} {}
   };

   template<typename K, typename V>
   struct ebo<K, V, false> {
      constexpr ebo() = default;
      constexpr explicit ebo(V const &v) : data_{v} {}
      constexpr ebo(ebo const &rhs) = default;
      template<typename T, typename = std::enable_if_t<!std::is_same_v<V, T>>>
      constexpr ebo(T const &rhs) : data_{static_cast<T const &>(rhs)} {}

      V data_;
   };

   template<typename K, typename V>
   constexpr auto ebo_get(ebo<K, V, true> const &x) -> V const & {
      return x;
   }

   template<typename K, typename V>
   constexpr auto ebo_get(ebo<K, V, false> const &x) -> V const & {
      return x.data_;
   }
}

TSL_NS_END

#endif //TRANS_DSL_2_EBO_H
