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
   template<typename IN>
   using type = type_list::Drop_t<IN, N>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template <size_t N>
struct DropRight {
   template<typename IN>
   using type = type_list::DropRight_t<IN, N>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template <size_t N>
struct TakeRight {
   template<typename IN>
   using type = type_list::TakeRight_t<IN, N>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template <size_t N>
struct Take {
   template<typename IN>
   using type = type_list::Take_t<IN, N>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template<size_t N>
struct Elem {
   template<typename IN>
   using type = type_list::Elem_t<IN, N>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template < template<typename>     typename F>
struct Transform {
   template<typename IN>
   using type = type_list::Transform_t<IN, F>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template< template<typename> typename     PRED>
struct Filter {
   template<typename IN>
   using type = type_list::Filter_t<IN, PRED>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename F>
struct FoldR {
   template<typename IN>
   using type = type_list::FoldR_t<IN, F>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename OP>
struct FoldROpt {
   template<typename IN>
   using type = type_list::FoldROpt_t<IN, OP>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename F>
struct FoldL {
   template<typename IN>
   using type = type_list::FoldL_t<IN, F>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template< template <typename, typename> typename OP>
struct FoldLOpt {
   template<typename IN>
   using type = type_list::FoldLOpt_t<IN, OP>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template
   < template <typename, typename> typename F
   , typename                               INIT>
struct FoldR_Init {
   template<typename IN>
   using type = type_list::FoldRInit_t<IN, F, INIT>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template
   < template <typename, typename> typename F
   , typename                               INIT>
struct FoldL_Init {
   template<typename IN>
   using type = type_list::FoldLInit_t<IN, F, INIT>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template
   < template <typename, typename> typename F
      , typename                               INIT>
struct FoldROptInit {
   template<typename IN>
   using type = type_list::FoldROptInit_t<IN, F, INIT>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template
   < template <typename, typename> typename F
      , typename                               INIT>
struct FoldLOptInit {
   template<typename IN>
   using type = type_list::FoldLOptInit_t<IN, F, INIT>;
};

/////////////////////////////////////////////////////////////////////////////////////////
struct Flatten {
   template<typename IN>
   using type = type_list::Flatten_t<IN>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template<typename LIST>
struct ZipWith {
   template<typename IN>
   using type = type_list::ZipWith_t<IN, LIST>;
};

/////////////////////////////////////////////////////////////////////////////////////////
template<typename IN>
class TypeStream final {

   template<typename T, typename = void>
   struct TypeListTrait {
      using type = TypeList<T>;
   };

   template<typename T>
   struct TypeListTrait<T, std::enable_if_t<std::is_base_of_v<TypeListSignature, T>>> {
      using type = T;
   };

   template<typename A_OP>
   struct GenericComposer {
      template<typename B_OP>
      struct Bind {
         template<typename INPUT>
         class Result {
            using output = typename TypeListTrait<typename A_OP::template type<INPUT>>::type;
         public:
            using type = typename B_OP::template Result<output>::type;
         };
      };

      template<typename INPUT>
      struct Result {
         using type = typename A_OP::template type<INPUT>;
      };
   };

   //////////////////////////////////////////////////////////////////////////
   template<typename ... OPs>
   struct Compose;

   template<typename H, typename ... OPs>
   struct Compose<H, OPs...> {
      using type = typename GenericComposer<H>::template Bind<typename Compose<OPs...>::type>;
   };

   template<typename H>
   struct Compose<H> {
      using type = GenericComposer<H>;
   };

public:
   template<typename ... OPs>
   using _ooo_ = typename Compose<OPs...>::type::template Result<IN>::type;
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

#define __TL_Stream_type ::template output<CUB_NS::details::__StUpId_ReSuLt_TrAiT>::type
#define __TL_Pipeline(stream, ...) typename CUB_NS::TypeStream<CUB_NS::ListWrapper<stream>>::template _ooo_<__VA_ARGS__>
#define __TL_Pipeline_t(stream, ...) __TL_Pipeline(stream, __VA_ARGS__)
#define __TL_Raw_Pipeline__(stream, ...) __TL_Pipeline(CUB_NS::TypeList<stream>, __VA_ARGS__)
#define __TL_Raw_Pipeline_t(stream, ...) __TL_Raw_Pipeline__(stream, __VA_ARGS__)

#define __TL_OutputTo(result) ::template output<result>

#endif //TRANS_DSL_2_TYPELISTPIPELINE_H
