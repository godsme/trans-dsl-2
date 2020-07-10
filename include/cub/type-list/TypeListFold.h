//
// Created by Darwin Yuan on 2020/7/10.
//

#ifndef TRANS_DSL_2_TYPELISTFOLD_H
#define TRANS_DSL_2_TYPELISTFOLD_H

#include <cub/cub_ns.h>

CUB_NS_BEGIN

namespace details {
   template<template<typename, typename> typename F, typename INIT, typename ... Ts>
   struct FoldR {
      using type = INIT;
   };

   template<template<typename, typename> typename F, typename INIT, typename H, typename ... Ts>
   struct FoldR<F, INIT, H, Ts...> {
      using type = typename F<H, typename FoldR<F, INIT, Ts...>::type>::type;
   };

   template<template<typename, typename> typename F, typename H, typename H1, typename ... Ts>
   struct FoldR<F, void, H, H1, Ts...> {
      using type = typename F<H, typename FoldR<F, void, H1, Ts...>::type>::type;
   };

   template<template<typename, typename> typename F, typename H, typename ... Ts>
   struct FoldR<F, void, H, Ts...> {
      using type = H;
   };
}

namespace details {
   template<
      template <typename, typename> typename F,
      typename                               T1,
      typename                               T2>
   struct CombineOpt {
      using type = typename F<T1, T2>::type;
   };

   template<
      template <typename, typename> typename F>
   struct CombineOpt<F, void, void> {
      using type = void;
   };

   template<
      template <typename, typename> typename F,
      typename                               T1>
   struct CombineOpt<F, T1, void> {
      using type = T1;
   };

   template<
      template <typename, typename> typename F,
      typename                               T2>
   struct CombineOpt<F, void, T2> {
      using type = T2;
   };

   template
      < template <typename, typename> typename F
      , typename                               INIT
      , typename                           ... Ts>
   class FoldROpt {
      template<typename T1, typename T2>
      using OptF = CombineOpt<F, T1, T2>;
   public:
      using type = typename FoldR<OptF, INIT, Ts...>::type;
   };
}

template
   < template <typename, typename> typename F
   , typename                               INIT
   , typename                           ... Ts>
using FoldR_Init_t = typename details::FoldR<F, INIT, Ts...>::type;

template< template <typename, typename> typename F, typename  ... Ts>
using FoldR_t = FoldR_Init_t<F, void, Ts...>;


template
   < template <typename, typename> typename F
   , typename                               INIT
   , typename                           ... Ts>
using FoldROptInit_t = typename details::FoldROpt<F, INIT, Ts...>::type;

template< template <typename, typename> typename F, typename ... Ts>
using FoldROpt_t = FoldROptInit_t<F, void, Ts...>;

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTFOLD_H
