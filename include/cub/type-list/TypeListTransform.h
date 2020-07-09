//
// Created by Darwin Yuan on 2020/7/8.
//

#ifndef TRANS_DSL_2_TYPELISTTRANSFORM_H
#define TRANS_DSL_2_TYPELISTTRANSFORM_H

#include <cub/cub_ns.h>
#include <cub/type-list/TypeList.h>
#include <type_traits>

CUB_NS_BEGIN

namespace details {
   template<
      typename IN,
      template<typename> typename F,
      template<typename ...> typename RESULT,
      typename = void,
      typename ... OUT>
   struct Transform {
      using type = RESULT<OUT...>; // 将最终结果输出给 RESULT
   };

   template<
      typename IN,
      template<typename> typename F,
      template<typename ...> typename RESULT,
      typename ... OUT>
   struct Transform<IN, F, RESULT, std::void_t<typename IN::Head>, OUT...> {
      using type =
      typename Transform<
         typename IN::Tail,
         F,
         RESULT,
         void,
         __TYPE_LIST_APPEND(OUT..., typename F<typename IN::Head>::type)
      >::type;
   };
}

///////////////////////////////////////////////////////////////////////////////////////
template<
   template<typename>     typename F,
   template<typename ...> typename RESULT,
   typename                    ... IN>
using Transform_t =
   typename details::Transform<
      TypeList < IN...>,
      F,
      RESULT,
      void
      __EMPTY_OUTPUT_TYPE_LIST___
   >::type;

///////////////////////////////////////////////////////////////////////////////////////
template<
   template<typename>     typename F,
   template<typename ...> typename  RESULT,
   typename    ... IN>
using Transform_tt = typename Transform_t<F, RESULT, IN...>::type;

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTTRANSFORM_H
