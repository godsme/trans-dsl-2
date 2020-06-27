//
// Created by Darwin Yuan on 2020/6/28.
//

#ifndef TRANS_DSL_2_TYPEEXTRACTOR_H
#define TRANS_DSL_2_TYPEEXTRACTOR_H

#include <trans-dsl/tsl_ns.h>
#include <cstddef>
#include <type_traits>

TSL_NS_BEGIN

namespace details {
   template<size_t N, typename ... Ts>
   struct TypeExtractor;

   template<size_t N, typename Head, typename ... Ts>
   struct TypeExtractor<N, Head, Ts...> {
      using type = typename TypeExtractor<N-1, Ts...>::type;
   };

   template<typename Head, typename ... Ts>
   struct TypeExtractor<0, Head, Ts...> {
      using type = Head;
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_TYPEEXTRACTOR_H
