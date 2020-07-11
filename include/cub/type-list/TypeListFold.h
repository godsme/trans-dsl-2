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

////////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename OP, typename  ... Ts>
using FoldR_t = typename details::FoldR<OP, Ts...>::type;

template
   < template <typename, typename> typename OP
   , typename                               INIT
   , typename                           ... Ts>
using FoldRInit_t = FoldR_t<OP, Ts..., INIT>;

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
using FoldLInit_t = FoldL_t<OP, INIT, Ts...>;

template< template <typename, typename> typename OP, typename ... Ts>
using FoldLOpt_t = typename details::FoldOpt<OP>::template FoldL<Ts...>;;

template
   < template <typename, typename> typename OP
   , typename                            INIT
   , typename                           ... Ts>
using FoldLOptInit_t = FoldLOpt_t<OP, INIT, Ts...>;;

namespace type_list{

   template<template <typename, typename> typename OP>
   struct Fold {
      template<typename ... Ts>
      using FoldR_Result = typename details::FoldR<OP, Ts...>::type;
      template<typename IN>
      using FoldR_t = typename IN::template output<FoldR_Result>;

      template<typename ... Ts>
      using FoldROpt_Result = typename details::FoldOpt<OP>::template FoldR<Ts...>;
      template<typename IN>
      using FoldROpt_t = typename IN::template output<FoldROpt_Result>;

      template<typename ... Ts>
      using FoldL_Result = typename details::FoldL<OP, Ts...>::type;
      template<typename IN>
      using FoldL_t = typename IN::template output<FoldL_Result>;

      template<typename ... Ts>
      using FoldLOpt_Result = typename details::FoldOpt<OP>::template FoldL<Ts...>;
      template<typename IN>
      using FoldLOpt_t = typename IN::template output<FoldLOpt_Result>;
   };

   template<typename  IN, template <typename, typename> typename OP>
   using FoldR_t = typename Fold<OP>::template FoldR_t<IN>;

   template<typename  IN, template <typename, typename> typename OP>
   using FoldROpt_t = typename Fold<OP>::template FoldROpt_t<IN>;

   template<typename  IN, template <typename, typename> typename OP>
   using FoldL_t = typename Fold<OP>::template FoldL_t<IN>;

   template<typename  IN, template <typename, typename> typename OP>
   using FoldLOpt_t = typename Fold<OP>::template FoldLOpt_t<IN>;

   template
      < typename                               IN
         , template <typename, typename> typename OP
         , typename                               INIT>
   using FoldLInit_t = typename Fold<OP>::template FoldL_t<typename IN::template preappend<INIT>>;

   template
      < typename                               IN
         , template <typename, typename> typename OP
         , typename                               INIT>
   using FoldRInit_t = typename Fold<OP>::template FoldR_t<typename IN::template append<INIT>>;

   template
      < typename                               IN
         , template <typename, typename> typename OP
         , typename                               INIT>
   using FoldLOptInit_t = typename Fold<OP>::template FoldLOpt_t<typename IN::template preappend<INIT>>;

   template
      < typename                               IN
      , template <typename, typename> typename OP
      , typename                               INIT>
   using FoldROptInit_t = typename Fold<OP>::template FoldROpt_t<typename IN::template append<INIT>>;
}

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTFOLD_H
