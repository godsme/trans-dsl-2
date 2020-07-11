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
#include <cub/type-list/Flattenable.h>
#include <cub/type-list/TypeListZip.h>

CUB_NS_BEGIN

/////////////////////////////////////////////////////////////////////////////////////////
template <size_t N>
struct Drop {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = Drop_t<N, RESULT, Ts...>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template <size_t N>
struct DropRight {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = DropRight_t<N, RESULT, Ts...>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template <size_t N>
struct TakeRight {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = TakeRight_t<N, RESULT, Ts...>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template <size_t N>
struct Take {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = Take_t<N, RESULT, Ts...>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template<size_t N>
struct Elem {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = RESULT<Elem_t<N, Ts...>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template < template<typename>     typename F>
struct Transform {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = Transform_t<F, RESULT, Ts...>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template< template<typename> typename     PRED>
struct Filter {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = Filter_t<PRED, RESULT, Ts...>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename F>
struct FoldR {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = RESULT<FoldR_t<F, Ts...>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename OP>
struct FoldROpt {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = RESULT<FoldROpt_t<OP, Ts...>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename F>
struct FoldL {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = RESULT<FoldL_t<F, Ts...>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename OP>
struct FoldLOpt {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = RESULT<FoldLOpt_t<OP, Ts...>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template
   < template <typename, typename> typename F
   , typename                               INIT>
struct FoldR_Init {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = RESULT<FoldRInit_t<F, INIT, Ts...>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template
   < template <typename, typename> typename F
   , typename                               INIT>
struct FoldL_Init {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = RESULT<FoldLInit_t<F, INIT, Ts...>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template
   < template <typename, typename> typename F
      , typename                               INIT>
struct FoldROptInit {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = RESULT<FoldROptInit_t<F, INIT, Ts...>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template
   < template <typename, typename> typename F
      , typename                               INIT>
struct FoldLOptInit {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = RESULT<FoldLOptInit_t<F, INIT, Ts...>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
struct Flatten {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = Flatten_t<RESULT, Ts...>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template<typename LIST>
struct ZipWith {
   template<template<typename ...> typename RESULT, typename ... Ts>
   using type = ZipWith_t<LIST, RESULT, Ts...>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template<typename ... Types>
class TypeStream final {
   template<typename OP>
   struct GenericComposer {
      template<typename ANOTHER>
      struct Compose {
         template<typename ... Ts>
         struct Result {
            template<template<typename ...> typename RESULT>
            using type = typename OP::template type<ANOTHER::template Result, Ts...>::template type<RESULT>;
         };
      };

      template<typename ... Ts>
      struct Result {
         template<template<typename ...> typename RESULT>
         using type = typename OP::template type<RESULT, Ts...>;
      };
   };

   //////////////////////////////////////////////////////////////////////////
   template<typename ... OPs>
   struct Compose;

   template<typename H, typename ... OPs>
   struct Compose<H, OPs...> {
      using type = typename GenericComposer<H>::template Compose<typename Compose<OPs...>::type>;
   };

   template<typename H>
   struct Compose<H> {
      using type = GenericComposer<H>;
   };

public:
   template<typename ... OPs>
   struct _ooo_ {
      template<template<typename ...> typename RESULT>
      using output = typename Compose<OPs...>::type::template Result<Types...>::template type<RESULT>;
   };
};

namespace details {
   template<typename ... Ts>
   struct __StUpId_ReSuLt_TrAiT {
      static_assert(sizeof...(Ts) == 1);
   };

   template<typename H>
   struct __StUpId_ReSuLt_TrAiT<H> {
      using type = H;
   };
}

CUB_NS_END

#define __TL_Pipeline__(stream, ...) typename CUB_NS::TypeStream<stream>::template _ooo_<__VA_ARGS__>
#define __TL_Pipeline_t(stream, ...) \
__TL_Pipeline__(stream, __VA_ARGS__)::template output<CUB_NS::details::__StUpId_ReSuLt_TrAiT>::type
#define __TL_OutputTo__(result) ::template output<result>

#endif //TRANS_DSL_2_TYPELISTPIPELINE_H
