//
// Created by Darwin Yuan on 2020/7/8.
//

#ifndef TRANS_DSL_2_TYPELISTSPLIT_H
#define TRANS_DSL_2_TYPELISTSPLIT_H

#include <cub/cub_ns.h>
#include <cstddef>
#include <cub/type-list/TypeList.h>

CUB_NS_BEGIN

namespace details {
   template<
      size_t N,
      typename IN,
      template<typename ...> typename RESULT,
      typename ... OUT>
   struct Split {
      using type = typename Split<
         N - 1,
         typename IN::Tail,
         RESULT,
         OUT..., typename IN::Head
      >::type;
   };

   template<
      typename IN,
      template<typename ...> typename RESULT,
      typename ... OUT>
   struct Split<0, IN, RESULT, OUT...> {
      struct type {
         using first  = RESULT<OUT...>;
         using second = typename IN::type;
      };
   };
}

//////////////////////////////////////////////////////////////////////
template<
   size_t N,
   template<typename ...> typename RESULT_1,
   template<typename ...> typename RESULT_2,
   typename ... IN>
using Split_t =
   typename details::Split<
      N,
      GenericTypeList<RESULT_2, IN...>,
      RESULT_1
      __EMPTY_OUTPUT_TYPE_LIST___
   >::type;

//////////////////////////////////////////////////////////////////////
template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using take_t =
   typename details::Split<
      N,
      TypeList<IN...>,
      RESULT
      __EMPTY_OUTPUT_TYPE_LIST___
   >::type::first;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using take_tt = typename take_t<N, RESULT, IN...>::type;

//////////////////////////////////////////////////////////////////////
template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using dropRight_t = take_t<sizeof...(IN) - N, RESULT, IN...>;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using dropRight_tt = typename dropRight_t<N, RESULT, IN...>::type;

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTSPLIT_H
