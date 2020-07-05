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

   template<typename ... Ts>
   struct TypeN {
   private:
      template<template<typename ...> typename TAIL_USER, typename ... Tss>
      struct FullTraits;

      template<template<typename ...> typename TAIL_USER, typename H, typename ... Tss>
      struct FullTraits<TAIL_USER, H, Tss...> {
         using head = H;
         using tail = TAIL_USER<Tss...>;
      };

      template<size_t N, template<typename ...> typename USER>
      struct RestTrait {
      private:
         template<typename ... Tss>
         using Full = FullTraits<USER, Tss...>;

      public:
         using type = typename TypeListExtractor<N, Full, Ts...>::Type;
      };

   public:
      template<size_t N>
      using head = TypeListExtractor_t<N, HeadTraits, Ts...>;

      template<size_t N, template<typename ...> typename USER>
      using full = typename RestTrait<N, USER>::type;
   };
}

namespace details {
   template<
      size_t N,
      size_t M,
      template<typename ...> typename FIRST,
      template<typename ...> typename SECOND,
      typename EXTRACTOR,
      typename ... RESULT>
   struct TypeListSplitter {
   private:
      using elem = typename EXTRACTOR::template head<M>;
   public:
      using type = typename TypeListSplitter<N - 1, M + 1, FIRST, SECOND, EXTRACTOR, RESULT..., elem>::type;
   };

   template<
      size_t M,
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
}

namespace details {
   template<
      size_t N,
      template<typename ...> typename FIRST,
      template<typename ...> typename LAST,
      typename ... Ts>
   struct Splitter {
      static_assert(N < sizeof...(Ts), "N is out of scope");
      using type = typename TypeListSplitter<N, 0, FIRST, LAST, TypeN<Ts...>>::type;
   };

   template<typename ...>
   struct ___DoNothing___ {};

   template<
      size_t N,
      template<typename ...> typename FIRST,
      typename ... Ts>
   struct Taker {
      static_assert(N > 0, "N should not be 0");
      using type = typename Splitter<N-1, FIRST, ___DoNothing___, Ts...>::type;
   };


   template<size_t N, template<typename ...> typename FIRST, template<typename ...> typename LAST, typename ... Ts>
   using Split_t = typename Splitter<N, FIRST, LAST, Ts...>::type;

   template<size_t N, template<typename ...> typename USER, typename ... Ts>
   using Take_t = typename Taker<N, USER, Ts...>::type::first;

   template<size_t N, template<typename ...> typename USER, typename ... Ts>
   using DropRight_t = Take_t<sizeof...(Ts) - N, USER, Ts...>;
}

TSL_NS_END

#endif //TRANS_DSL_2_TYPELISTSPLITER_H
