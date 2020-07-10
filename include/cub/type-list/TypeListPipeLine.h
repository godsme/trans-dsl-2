//
// Created by Darwin Yuan on 2020/7/10.
//

#ifndef TRANS_DSL_2_TYPELISTPIPELINE_H
#define TRANS_DSL_2_TYPELISTPIPELINE_H

#include <cub/cub_ns.h>
#include <cub/type-list/TypeListTakeRight.h>
#include <cub/type-list/TypeListSplit.h>
#include <cub/type-list/TypeListTransform.h>
#include <cub/type-list/TypeListFold.h>
#include <cub/type-list/TypeListFilter.h>

CUB_NS_BEGIN

/////////////////////////////////////////////////////////////////////////////////////////
namespace details {

   template<size_t N, template<size_t, template<typename ...> typename, typename ...> typename OP>
   struct GenericDrop {
      template<typename ANOTHER>
      struct Bind {
         template<typename ... Ts>
         struct Result {
            template<template<typename ...> typename RESULT>
            using type = typename OP<N, ANOTHER::template Result, Ts...>::template type<RESULT>;
         };
      };

      template<typename ... Ts>
      struct Result {
         template<template<typename ...> typename RESULT>
         using type = OP<N, RESULT, Ts...>;
      };
   };
}

template <size_t N>
using Drop = details::GenericDrop<N, Drop_t>;

template <size_t N>
using DropRight = details::GenericDrop<N, DropRight_t>;

template <size_t N>
using TakeRight = details::GenericDrop<N, TakeRight_t>;

template <size_t N>
using Take = details::GenericDrop<N, Take_t>;

/////////////////////////////////////////////////////////////////////////////////////////
template<size_t N>
struct Elem {
   template<typename ... Ts>
   struct Result {
      template<template <typename ...> typename>
      using type = Elem_t <N, Ts...>;
   };
};

/////////////////////////////////////////////////////////////////////////////////////////
//template<template<typename>     typename F>
//struct Transform {
//   template<typename ANOTHER>
//   struct Bind {
//      template<typename ... Ts>
//      struct Result {
//         template <template<typename ...> typename RESULT>
//         using type = typename Transform_t<F, ANOTHER::template Result, Ts...>::template type<RESULT>;
//      };
//   };
//
//   template<typename ... Ts>
//   struct Result {
//      template<template <typename ...> typename RESULT>
//      using type = Transform_t<F, RESULT, Ts...>;
//   };
//};

namespace details {
   template
      <template<typename> typename F,
         template<template<typename> typename,
         template<typename ...> typename, typename ...> typename OP>
   struct GenericTransform {
      template<typename ANOTHER>
      struct Bind {
         template<typename ... Ts>
         struct Result {
            template<template<typename ...> typename RESULT>
            using type = typename OP<F, ANOTHER::template Result, Ts...>::template type<RESULT>;
         };
      };

      template<typename ... Ts>
      struct Result {
         template<template<typename ...> typename RESULT>
         using type = OP<F, RESULT, Ts...>;
      };
   };
}

template < template<typename>     typename F>
using Transform = details::GenericTransform<F, Transform_t>;

template < template<typename>     typename F>
using Filter = details::GenericTransform<F, Filter_t>;

/////////////////////////////////////////////////////////////////////////////////////////
template<template <typename, typename> typename F>
struct FoldR {
   template<typename ... Ts>
   struct Result {
      template<template <typename ...> typename>
      using type = FoldR_t <F, Ts...>;
   };
};

/////////////////////////////////////////////////////////////////////////////////////////
template<template <typename, typename> typename F>
struct FoldROpt {
   template<typename ... Ts>
   struct Result {
      template<template <typename ...> typename>
      using type = FoldROpt_t <F, Ts...>;
   };
};

/////////////////////////////////////////////////////////////////////////////////////////
template
   < template <typename, typename> typename F
   , typename                               INIT>
struct FoldR_Init {
   template<typename ... Ts>
   struct Result {
      template<template <typename ...> typename>
      using type = FoldR_Init_t <F, INIT, Ts...>;
   };
};

/////////////////////////////////////////////////////////////////////////////////////////
template<typename ... Ts>
struct TypeStream {
   template<typename ... OPs>
   struct Bind;

   template<typename H, typename ... OPs>
   struct Bind<H, OPs...> {
      using type = typename H::template Bind<typename Bind<OPs...>::type>;
   };

   template<typename H>
   struct Bind<H> {
      using type = H;
   };

   template<typename ...>
   struct __stupid {};

   template<typename ... OPs>
   struct _ooo_ {
      template<template<typename ...> typename RESULT>
      using output = typename Bind<OPs...>::type::template Result<Ts...>::template type<RESULT>;
      using type = typename Bind<OPs...>::type::template Result<Ts...>::template type<__stupid>;
   };
};

#define __TypeStream__(stream, ...) typename CUB_NS::TypeStream<stream>::template _ooo_<__VA_ARGS__>
#define __TypeStream_t(stream, ...) __TypeStream__(stream, __VA_ARGS__)::type
#define __ts_output_to__(result) ::template output<result>

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTPIPELINE_H
