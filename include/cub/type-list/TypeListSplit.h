//
// Created by Darwin Yuan on 2020/7/8.
//

#ifndef TRANS_DSL_2_TYPELISTSPLIT_H
#define TRANS_DSL_2_TYPELISTSPLIT_H

#include <cub/cub_ns.h>
#include <cstddef>
#include <cub/type-list/TypeList.h>
#include <cub/type-list/Tuple.h>

CUB_NS_BEGIN

namespace details {
   template<
      size_t N,
      typename IN,
      typename ... OUT>
   struct Split {
      template
         < template <typename ...> typename RESULT_1
         , template <typename ...> typename RESULT_2 >
      using output = typename Split<
            N - 1,
            typename IN::Tail,
            __TYPE_LIST_APPEND(OUT..., typename IN::Head)
         >::template output<RESULT_1, RESULT_2>;
   };

   template<
      typename IN,
      typename ... OUT>
   struct Split<0, IN, OUT...> {
      template
         < template <typename ...> typename RESULT_1
         , template <typename ...> typename RESULT_2 >
      using output = __TL_make_pair(
         RESULT_1<OUT...>,
         typename IN::template output<RESULT_2>);
   };

   template<typename ...>
   struct __never_used_dummy__ {};
}

namespace type_list {
   template<
      size_t N,
      template<typename ...> typename RESULT_1,
      template<typename ...> typename RESULT_2,
      typename       TYPE_LIST>
   using Split_t =
   typename details::Split<
      N,
      TYPE_LIST
      __EMPTY_OUTPUT_TYPE_LIST___
   >::template output<RESULT_1, RESULT_2>;

   //////////////////////////////////////////////////////////////////////
   template<
      size_t N,
      template<typename ...> typename RESULT,
      typename       TYPE_LIST>
   using Take_t =
   typename details::Split<
      N,
      TYPE_LIST
      __EMPTY_OUTPUT_TYPE_LIST___
   >::template output<RESULT, details::__never_used_dummy__>::first;
}

//////////////////////////////////////////////////////////////////////
template<
   size_t N,
   template<typename ...> typename RESULT_1,
   template<typename ...> typename RESULT_2,
   typename ... IN>
using Split_t = type_list::Split_t<N, RESULT_1, RESULT_2, TypeList<IN...>>;

//////////////////////////////////////////////////////////////////////
template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using Take_t = type_list::Take_t<N, RESULT, TypeList<IN...>>;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using Take_tt = typename Take_t<N, RESULT, IN...>::type;

//////////////////////////////////////////////////////////////////////
namespace details {
   template<
      size_t N,
      template<typename ...> typename RESULT,
      typename       TYPE_LIST>
   struct DropRight {
      static_assert(N >= TYPE_LIST::size, "N is greater than the size of type list");
      using type = type_list::Take_t<TYPE_LIST::size - N, RESULT, TYPE_LIST>;
   };
}

namespace type_list {
   template<
      size_t N,
      template<typename ...> typename RESULT,
      typename       TYPE_LIST>
   using DropRight_t = typename details::DropRight<N, RESULT, TYPE_LIST>::type;

   template<
      size_t N,
      template<typename ...> typename RESULT,
      typename       TYPE_LIST>
   using DropRight_tt = DropRight_t<N, RESULT, TYPE_LIST>;
}

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using DropRight_t = type_list::DropRight_t<N, RESULT, TypeList<IN...>>;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using DropRight_tt = typename DropRight_t<N, RESULT, IN...>::type;

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTSPLIT_H
