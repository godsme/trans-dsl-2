//
// Created by Darwin Yuan on 2020/7/10.
//

#ifndef TRANS_DSL_2_TYPELISTFOLD_H
#define TRANS_DSL_2_TYPELISTFOLD_H

#include <cub/cub_ns.h>

CUB_NS_BEGIN

namespace details {
   template<template<typename, typename> typename OP, typename ... Ts>
   struct FoldR;

   template<template<typename, typename> typename OP, typename ACC, typename ... Ts>
   struct FoldR<OP, ACC, Ts...> {
      using type = ACC;
   };

   template<template<typename, typename> typename OP, typename ACC, typename H, typename ... Ts>
   struct FoldR<OP, ACC, H, Ts...> {
      using type = typename OP<ACC, typename FoldR<OP, H, Ts...>::type>::type;
   };
}

namespace details {
   template<template<typename, typename> typename OP, typename ... Ts>
   struct FoldL;

   template<template<typename, typename> typename OP, typename ACC, typename ... Ts>
   struct FoldL<OP, ACC, Ts...> {
      using type = ACC;
   };

   template<template<typename, typename> typename OP, typename ACC, typename H, typename ... Ts>
   struct FoldL<OP, ACC, H, Ts...> {
      using type = typename FoldL<OP, typename OP<ACC, H>::type, Ts...>::type;
   };
}

namespace details {
   template<
      template <typename, typename> typename OP,
      typename                               T1,
      typename                               T2>
   struct CombineOpt {
      using type = typename OP<T1, T2>::type;
   };

   template<
      template <typename, typename> typename OP>
   struct CombineOpt<OP, void, void> {
      using type = void;
   };

   template<
      template <typename, typename> typename OP,
      typename                               T1>
   struct CombineOpt<OP, T1, void> {
      using type = T1;
   };

   template<
      template <typename, typename> typename OP,
      typename                               T2>
   struct CombineOpt<OP, void, T2> {
      using type = T2;
   };

   template
      < template <typename, typename> typename OP>
   class FoldOpt {
      template<typename T1, typename T2>
      using OptF = CombineOpt<OP, T1, T2>;
   public:
      template<typename ... Ts>
      using FoldR = typename FoldR<OptF, Ts...>::type;

      template<typename ... Ts>
      using FoldL = typename FoldL<OptF, Ts...>::type;
   };
}

//////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename OP, typename  ... Ts>
using FoldR_t = typename details::FoldR<OP, Ts...>::type;

template
   < template <typename, typename> typename OP
   , typename                               INIT
   , typename                           ... Ts>
using FoldR_Init_t = FoldR_t<OP, Ts..., INIT>;

template< template <typename, typename> typename OP, typename ... Ts>
using FoldROpt_t = typename details::FoldOpt<OP>::template FoldR<Ts...>;;

template
   < template <typename, typename> typename OP
      , typename                               INIT
      , typename                           ... Ts>
using FoldROptInit_t = FoldROpt_t<OP, Ts..., INIT>;

//////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename OP, typename  ... Ts>
using FoldL_t = typename details::FoldL<OP, Ts...>::type;

template
   < template <typename, typename> typename OP
   , typename                               INIT
   , typename                           ... Ts>
using FoldL_Init_t = FoldL_t<OP, INIT, Ts...>;

template< template <typename, typename> typename OP, typename ... Ts>
using FoldLOpt_t = typename details::FoldOpt<OP>::template FoldL<Ts...>;;

template
   < template <typename, typename> typename OP
   , typename                            INIT
   , typename                           ... Ts>
using FoldLOptInit_t = FoldLOpt_t<OP, INIT, Ts...>;;

namespace type_list{
   template< template <typename, typename> typename OP, typename  TYPE_LIST>
   struct Fold {
      template<typename ... Ts>
      using FoldR_Result = typename details::FoldR<OP, Ts...>::type;
      using FoldR_t = typename TYPE_LIST::template output<FoldR_Result>;

      template<typename ... Ts>
      using FoldROpt_Result = typename details::FoldOpt<OP>::template FoldR<Ts...>;
      using FoldROpt_t = typename TYPE_LIST::template output<FoldROpt_Result>;

      template<typename ... Ts>
      using FoldL_Result = typename details::FoldL<OP, Ts...>::type;
      using FoldL_t = typename TYPE_LIST::template output<FoldL_Result>;

      template<typename ... Ts>
      using FoldLOpt_Result = typename details::FoldOpt<OP>::template FoldL<Ts...>;
      using FoldLOpt_t = typename TYPE_LIST::template output<FoldLOpt_Result>;
   };

   template< template <typename, typename> typename OP, typename  TYPE_LIST>
   using FoldR_t = typename Fold<OP, TYPE_LIST>::FoldR_t;

   template< template <typename, typename> typename OP, typename  TYPE_LIST>
   using FoldROpt_t = typename Fold<OP, TYPE_LIST>::FoldROpt_t;

   template< template <typename, typename> typename OP, typename  TYPE_LIST>
   using FoldL_t = typename Fold<OP, TYPE_LIST>::FoldL_t;

   template< template <typename, typename> typename OP, typename  TYPE_LIST>
   using FoldLOpt_t = typename Fold<OP, TYPE_LIST>::FoldLOpt_t;

   template
      < template <typename, typename> typename OP
      , typename                               INIT
      , typename                               TYPE_LIST>
   using FoldL_Init_t = typename Fold<OP, typename TYPE_LIST::template preappend<INIT>>::FoldL_t;

   template
      < template <typename, typename> typename OP
      , typename                               INIT
      , typename                               TYPE_LIST>
   using FoldR_Init_t = typename Fold<OP, typename TYPE_LIST::template append<INIT>>::FoldR_t;

   template
      < template <typename, typename> typename OP
      , typename                               INIT
      , typename                               TYPE_LIST>
   using FoldLOptInit_t = typename Fold<OP, typename TYPE_LIST::template preappend<INIT>>::FoldLOpt_t;

   template
      < template <typename, typename> typename OP
      , typename                               INIT
      , typename                               TYPE_LIST>
   using FoldROptInit_t = typename Fold<OP, typename TYPE_LIST::template append<INIT>>::FoldROpt_t;
}

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTFOLD_H
