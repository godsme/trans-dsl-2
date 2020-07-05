//
// Created by Darwin Yuan on 2020/6/28.
//

#ifndef TRANS_DSL_2_TYPELISTEXTRACTOR_H
#define TRANS_DSL_2_TYPELISTEXTRACTOR_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/SeqInt.h>
#include <cstddef>

TSL_NS_BEGIN

namespace details {
   template<
      size_t N,
      template<typename ...> typename TRAIT,
      typename ... Ts>
   struct TypeListExtractor;

   template<
      size_t N,
      template<typename ...> typename TRAIT,
      typename HEAD,
      typename ... TAIL>
   struct TypeListExtractor<N, TRAIT, HEAD, TAIL...> {
      using type = typename TypeListExtractor<SeqInt(N - 1), TRAIT, TAIL...>::type;
   };

   template<
      template<typename ...> typename TRAIT,
      typename HEAD,
      typename ... TAIL>
   struct TypeListExtractor<0, TRAIT, HEAD, TAIL...> {
      using type = TRAIT<HEAD, TAIL...>;
   };

   template<
      size_t N,
      template<typename ...> typename TRAIT,
      typename ... Ts>
    using TypeListExtractor_t = typename TypeListExtractor<N, TRAIT, Ts...>::type::type;

   //////////////////////////////////////////////////
   template<typename ...> struct HeadTraits;

   template<typename HEAD, typename ...TAIL>
   struct HeadTraits<HEAD, TAIL...> final {
      using type = HEAD;
   };

   template<template<typename ...> typename USER>
   struct TailTraits {
      template<typename ... TAIL>
      struct Inner;

      template< typename HEAD, typename ...TAIL>
      struct Inner<HEAD, TAIL...> {
         using type = USER<TAIL...>;
      };
   };

   template<size_t N, template<typename ...> typename USER, typename ... Ts>
   class RightTaker final {
      static_assert(N > 0, "N should not be 0");
      static_assert(N < sizeof...(Ts), "N is out of scope");
      template<typename ... Tss>
      using Traits = typename TailTraits<USER>::template Inner<Tss...>;
   public:
      using type = typename TypeListExtractor<sizeof...(Ts) - N, Traits, Ts...>::type;
   };

   template<size_t N, template<typename ...> typename USER, typename ... Ts>
   using TakeRight_t = typename RightTaker<N, USER, Ts...>::type;

   template<size_t N, template<typename ...> typename USER, typename ... Ts>
   using Drop_t = TakeRight_t<sizeof...(Ts) - N, USER, Ts...>;
}

TSL_NS_END

#endif //TRANS_DSL_2_TYPELISTEXTRACTOR_H
