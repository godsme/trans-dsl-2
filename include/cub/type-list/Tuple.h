//
// Created by Darwin Yuan on 2020/7/11.
//

#ifndef TRANS_DSL_2_TUPLE_H
#define TRANS_DSL_2_TUPLE_H

#include <cub/cub_ns.h>

CUB_NS_BEGIN


class Pair final {
   template <typename T1, typename T2>
   struct TypePair {
      using first = T1;
      using second = T2;
   };

   template <auto V1, auto V2>
   struct ValuePair {
      constexpr static auto first  = V1;
      constexpr static auto second = V2;
   };

   template <typename T1, auto V2>
   struct TypeValuePair {
      using first = T1;
      constexpr static auto second = V2;
   };

   template <auto V1, typename T2>
   struct ValueTypePair {
      constexpr static auto first  = V1;
      using second = T2;
   };

public:
   template <typename T1, typename T2>
   static auto make() -> TypePair<T1, T2>;

   template <auto V1, auto V2>
   static auto make() -> ValuePair<V1, V2>;

   template <typename T1, auto V2>
   static auto make() -> TypeValuePair<T1, V2>;

   template <auto V1, typename T2>
   static auto make() -> ValueTypePair<V1, T2>;
};

#define __TL_make_pair(p1, p2) decltype(Pair::make<p1, p2>())

CUB_NS_END

#endif //TRANS_DSL_2_TUPLE_H
