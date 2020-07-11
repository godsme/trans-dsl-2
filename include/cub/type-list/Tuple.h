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
   template<typename T, auto V>
   struct TV {
      using first = T;
      constexpr static auto second = V;
   };

   template<typename T1, typename T2>
   struct TT {
      using first = T1;
      using second = T2;
   };

   template<auto V, typename T2>
   struct VT {
      constexpr static auto first  = V;
      using second = T2;
   };

   template<auto V1, auto V2>
   struct VV {
      constexpr static auto first   = V1;
      constexpr static auto second  = V2;
   };

   template <typename T1, typename T2, typename = void>
   struct Trait {
      using type = TT<T1, T2>;
   };

   template <typename T1, typename T2>
   struct Trait<T1, T2, std::enable_if_t<IsValue<T1> && IsValue<T2>>> {
      using type = VV<T1::value, T2::value>;
   };

   template <typename T1, typename T2>
   struct Trait<T1, T2, std::enable_if_t<IsValue<T1> && !IsValue<T2>>> {
      using type = VT<T1::value, T2>;
   };

   template <typename T1, typename T2>
   struct Trait<T1, T2, std::enable_if_t<!IsValue<T1> && IsValue<T2>>> {
      using type = TV<T1, T2::value>;
   };
};

#define __TL_make_pair(p1, p2) Pair::Trait<p1, p2>::type

CUB_NS_END

#endif //TRANS_DSL_2_TUPLE_H
