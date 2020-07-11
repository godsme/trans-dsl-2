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
   template<typename IN, template<typename ...> typename RESULT>
   using type = type_list::Drop_t<IN, N, RESULT>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template <size_t N>
struct DropRight {
   template<typename IN, template<typename ...> typename RESULT>
   using type = type_list::DropRight_t<IN, N, RESULT>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template <size_t N>
struct TakeRight {
   template<typename IN, template<typename ...> typename RESULT>
   using type = type_list::TakeRight_t<IN, N, RESULT>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template <size_t N>
struct Take {
   template<typename IN, template<typename ...> typename RESULT>
   using type = type_list::Take_t<IN, N, RESULT>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template<size_t N>
struct Elem {
   template<typename IN, template<typename ...> typename RESULT>
   using type = RESULT<type_list::Elem_t<IN, N>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template < template<typename>     typename F>
struct Transform {
   template<typename IN, template<typename ...> typename RESULT>
   using type = type_list::Transform_t<IN, F, RESULT>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template< template<typename> typename     PRED>
struct Filter {
   template<typename IN, template<typename ...> typename RESULT>
   using type = type_list::Filter_t<IN, PRED, RESULT>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename F>
struct FoldR {
   template<typename IN, template<typename ...> typename RESULT>
   using type = RESULT<type_list::FoldR_t<IN, F>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename OP>
struct FoldROpt {
   template<typename IN, template<typename ...> typename RESULT>
   using type = RESULT<type_list::FoldROpt_t<IN, OP>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename F>
struct FoldL {
   template<typename IN, template<typename ...> typename RESULT>
   using type = RESULT<type_list::FoldL_t<IN, F>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename OP>
struct FoldLOpt {
   template<typename IN, template<typename ...> typename RESULT>
   using type = RESULT<type_list::FoldLOpt_t<IN, OP>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template
   < template <typename, typename> typename F
   , typename                               INIT>
struct FoldR_Init {
   template<typename IN, template<typename ...> typename RESULT>
   using type = RESULT<type_list::FoldRInit_t<IN, F, INIT>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template
   < template <typename, typename> typename F
   , typename                               INIT>
struct FoldL_Init {
   template<typename IN, template<typename ...> typename RESULT>
   using type = RESULT<type_list::FoldLInit_t<IN, F, INIT>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template
   < template <typename, typename> typename F
      , typename                               INIT>
struct FoldROptInit {
   template<typename IN, template<typename ...> typename RESULT>
   using type = RESULT<type_list::FoldROptInit_t<IN, F, INIT>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template
   < template <typename, typename> typename F
      , typename                               INIT>
struct FoldLOptInit {
   template<typename IN, template<typename ...> typename RESULT>
   using type = RESULT<type_list::FoldLOptInit_t<IN, F, INIT>>;
};

/////////////////////////////////////////////////////////////////////////////////////////
struct Flatten {
   template<typename IN, template<typename ...> typename RESULT>
   using type = type_list::Flatten_t<IN, RESULT>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template<typename LIST>
struct ZipWith {
   template<typename IN, template<typename ...> typename RESULT>
   using type = type_list::ZipWith_t<IN, LIST, RESULT>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template<typename IN>
class TypeStream final {
   template<typename OP>
   struct GenericComposer {
      template<typename ANOTHER>
      struct Compose {
         template<typename INPUT>
         struct Result {
            template<typename ... Ts>
            struct OutputAdapter {
               using type = typename ANOTHER::template Result<TypeList<Ts...>>;
            };

            template<template<typename ...> typename RESULT>
            using type = typename OP::template type<INPUT, OutputAdapter>::type::template type<RESULT>;
         };
      };

      template<typename INPUT>
      struct Result {
         template<template<typename ...> typename RESULT>
         using type = typename OP::template type<INPUT, RESULT>;
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
      using output = typename Compose<OPs...>::type::template Result<IN>::template type<RESULT>;
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
#define __TL_Raw_Pipeline__(stream, ...) __TL_Pipeline__(CUB_NS::TypeList<stream>, __VA_ARGS__)
#define __TL_Raw_Pipeline_t(stream, ...) \
__TL_Raw_Pipeline__(stream, __VA_ARGS__)::template output<CUB_NS::details::__StUpId_ReSuLt_TrAiT>::type
#define __TL_OutputTo__(result) ::template output<result>

#endif //TRANS_DSL_2_TYPELISTPIPELINE_H
