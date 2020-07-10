//
// Created by Darwin Yuan on 2020/7/10.
//

#ifndef TRANS_DSL_2_TYPELISTFOLD_H
#define TRANS_DSL_2_TYPELISTFOLD_H

#include <cub/cub_ns.h>

CUB_NS_BEGIN

namespace details {
   template<template<typename, typename> typename F, typename ... Ts>
   struct FoldR;

   template<template<typename, typename> typename F, typename H, typename ... Ts>
   struct FoldR<F, H, Ts...> {
      using type = H;
   };

   template<template<typename, typename> typename F, typename H1, typename H2, typename ... Ts>
   struct FoldR<F, H1, H2, Ts...> {
      using type = typename F<H1, typename FoldR<F, H2, Ts...>::type>::type;
   };
}

namespace details {
   template<template<typename, typename> typename F, typename ... Ts>
   struct FoldL;

   template<template<typename, typename> typename F, typename R, typename ... Ts>
   struct FoldL<F, R, Ts...> {
      using type = R;
   };

   template<template<typename, typename> typename F, typename R, typename H, typename ... Ts>
   struct FoldL<F, R, H, Ts...> {
      using type = typename FoldL<F, typename F<R, H>::type, Ts...>::type;
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
      < template <typename, typename> typename F>
   class FoldOpt {
      template<typename T1, typename T2>
      using OptF = CombineOpt<F, T1, T2>;
   public:
      template<typename ... Ts>
      using FoldR = typename FoldR<OptF, Ts...>::type;

      template<typename ... Ts>
      using FoldL = typename FoldL<OptF, Ts...>::type;
   };
}

//////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename F, typename  ... Ts>
using FoldR_t = typename details::FoldR<F, Ts...>::type;

template
   < template <typename, typename> typename F
   , typename                               INIT
   , typename                           ... Ts>
using FoldR_Init_t = FoldR_t<F, Ts..., INIT>;

template< template <typename, typename> typename F, typename ... Ts>
using FoldROpt_t = typename details::FoldOpt<F>::template FoldR<Ts...>;;

template
   < template <typename, typename> typename F
      , typename                               INIT
      , typename                           ... Ts>
using FoldROptInit_t = FoldROpt_t<F, Ts..., INIT>;

//////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename F, typename  ... Ts>
using FoldL_t = typename details::FoldL<F, Ts...>::type;

template
   < template <typename, typename> typename F
   , typename                               INIT
   , typename                           ... Ts>
using FoldL_Init_t = FoldL_t<F, INIT, Ts...>;

template< template <typename, typename> typename F, typename ... Ts>
using FoldLOpt_t = typename details::FoldOpt<F>::template FoldL<Ts...>;;

template
   < template <typename, typename> typename F
   , typename                            INIT
   , typename                           ... Ts>
using FoldLOptInit_t = FoldLOpt_t<F, INIT, Ts...>;;

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTFOLD_H
