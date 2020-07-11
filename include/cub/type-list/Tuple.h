//
// Created by Darwin Yuan on 2020/7/11.
//

#ifndef TRANS_DSL_2_TUPLE_H
#define TRANS_DSL_2_TUPLE_H

#include <cub/cub_ns.h>
#include <type_traits>
#include <cub/type-list/TypeListValueWrapper.h>

CUB_NS_BEGIN


class Pair final {

   template<typename T>
   constexpr static bool IsValue = std::is_base_of_v<ValueWrapperSignature, T>;

public:
   template <typename T1, typename T2, typename = void, typename = void>
   struct type {
      using first = T1;
      using second = T2;
   };

   template <typename T1, typename T2>
   struct type<T1, T2, std::enable_if_t<IsValue<T1>>, std::enable_if_t<IsValue<T2>>> {
      constexpr static auto first  = T1::value;
      constexpr static auto second = T2::value;
   };

   template <typename T1, typename T2>
   struct type<T1, T2, std::enable_if_t<IsValue<T1>>, std::enable_if_t<!IsValue<T2>>> {
      constexpr static auto first = T1::value;
      using second = T2;
   };

   template <typename T1, typename T2>
   struct type<T1, T2, std::enable_if_t<!IsValue<T1>>, std::enable_if_t<IsValue<T2>>> {
      using first = T1;
      constexpr static auto second = T2::value;
   };
};

#define __TL_make_pair(p1, p2) Pair::type<p1, p2>

CUB_NS_END

#endif //TRANS_DSL_2_TUPLE_H
