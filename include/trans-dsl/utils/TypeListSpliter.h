//
// Created by Darwin Yuan on 2020/7/5.
//

#ifndef TRANS_DSL_2_TYPELISTSPLITER_H
#define TRANS_DSL_2_TYPELISTSPLITER_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/utils/TypeListExtractor.h>

TSL_NS_BEGIN

namespace details {

   template<template<typename ...> typename TAIL_USER, typename ... Ts>
   struct FullTraits;

   template<template<typename ...> typename TAIL_USER, typename H, typename ... Ts>
   struct FullTraits<TAIL_USER, H, Ts...> {
      using head = H;
      using tail = TAIL_USER<Ts...>;
   };

   template <typename ... Ts>
   struct TypeN {
   private:
      template <SeqInt N, template<typename ...> typename USER>
      struct RestTrait {
      private:
         template<typename ... Tss>
         using Full = FullTraits<USER, Tss...>;

      public:
         using type = typename TypeListExtractor<N, Full, Ts...>::Type;
      };

   public:
      template <SeqInt N>
      using head = TypeListExtractor_t<N, HeadTraits, Ts...>;

      template <SeqInt N, template<typename ...> typename USER>
      using full = typename RestTrait<N, USER>::type;
   };

   template<
      SeqInt N,
      SeqInt M,
      template<typename ...> typename FIRST,
      template<typename ...> typename SECOND,
      typename EXTRACTOR,
      typename ... RESULT>
   struct TypeListSplitter {
   private:
      using elem = typename EXTRACTOR::template head<M>;
   public:
      using type = typename TypeListSplitter<N-1, M+1, FIRST, SECOND, EXTRACTOR, RESULT..., elem>::type;
   };

   template<
      SeqInt M,
      template<typename ...> typename FIRST,
      template<typename ...> typename SECOND,
      typename EXTRACTOR,
      typename ... RESULT>
   struct TypeListSplitter<0, M, FIRST, SECOND, EXTRACTOR, RESULT...> {
   private:
      using full = typename EXTRACTOR::template full<M, SECOND>;
   public:
      struct type {
         using first = FIRST<RESULT..., typename full::head>;
         using second = typename full::tail;
      };
   };

   template<typename ...> struct ___DoNothing___ {};

   template<SeqInt N, template<typename ...> typename FIRST, template<typename ...> typename LAST, typename ... Ts>
   using Split_t = typename TypeListSplitter<N, 0, FIRST, LAST, TypeN<Ts...>>::type;

   template<SeqInt N, template<typename ...> typename USER, typename ... Ts>
   using Take_t = typename Split_t<N, USER, ___DoNothing___, Ts...>::first;
}

TSL_NS_END

#endif //TRANS_DSL_2_TYPELISTSPLITER_H
