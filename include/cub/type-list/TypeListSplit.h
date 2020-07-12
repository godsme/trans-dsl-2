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
      typename IN,
      size_t N,
      typename ... OUT>
   struct Split {
      template
         <template<typename ...> typename RESULT_1,
            template<typename ...> typename RESULT_2>
      using output = typename Split<
         typename IN::Tail,
         N - 1,
         __TYPE_LIST_APPEND(OUT..., typename IN::Head)
      >::template output<RESULT_1, RESULT_2>;
   };

   template<
      typename IN,
      typename ... OUT>
   struct Split<IN, 0, OUT...> {
      template
         <template<typename ...> typename RESULT_1,
            template<typename ...> typename RESULT_2>
      using output = __TL_make_pair(
         RESULT_1 < OUT...>,
         typename IN::template output<RESULT_2>);
   };
}

namespace details {
   template<
      typename IN,
      size_t N,
      typename ... OUT>
   struct Take {
      template< template <typename ...> typename RESULT>
      using output = typename Take<
         typename IN::Tail,
         N - 1,
         __TYPE_LIST_APPEND(OUT..., typename IN::Head)
      >::template output<RESULT>;
   };

   template<
      typename IN,
      typename ... OUT>
   struct Take<IN, 0, OUT...> {
      template< template <typename ...> typename RESULT>
      using output = RESULT<OUT...>;
   };
}

namespace type_list {
   template<
      typename       IN,
      size_t         N,
      template<typename ...> typename RESULT_1,
      template<typename ...> typename RESULT_2>
   using Split_t =
   typename details::Split<
      IN,
      N
      __EMPTY_OUTPUT_TYPE_LIST___
   >::template output<RESULT_1, RESULT_2>;

   //////////////////////////////////////////////////////////////////////
   template<
      typename       IN,
      size_t         N,
      template<typename ...> typename RESULT>
   using Take_t =
   typename details::Take<
      IN,
      N
      __EMPTY_OUTPUT_TYPE_LIST___
   >::template output<RESULT>;
}

//////////////////////////////////////////////////////////////////////
template<
   size_t N,
   template<typename ...> typename RESULT_1,
   template<typename ...> typename RESULT_2,
   typename ... IN>
using Split_t = type_list::Split_t<TypeList<IN...>, N, RESULT_1, RESULT_2>;

//////////////////////////////////////////////////////////////////////
template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using Take_t = type_list::Take_t<TypeList<IN...>, N, RESULT>;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using Take_tt = typename Take_t<N, RESULT, IN...>::type;

//////////////////////////////////////////////////////////////////////
namespace details {
   template<
      typename IN,
      size_t N,
      template<typename ...> typename RESULT>
   struct DropRight {
      static_assert(N >= IN::size, "N is greater than the size of type list");
      using type = type_list::Take_t<IN, IN::size - N, RESULT>;
   };
}

namespace type_list {
   template<
      typename        IN,
      size_t          N,
      template<typename ...> typename RESULT>
   using DropRight_t = typename details::DropRight<IN, N, RESULT>::type;

   template<
      typename       IN,
      size_t N,
      template<typename ...> typename RESULT>
   using DropRight_tt = DropRight_t<IN, N, RESULT>;
}

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using DropRight_t = type_list::DropRight_t<TypeList<IN...>, N, RESULT>;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using DropRight_tt = typename DropRight_t<N, RESULT, IN...>::type;

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTSPLIT_H
