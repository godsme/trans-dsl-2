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
      typename INPUT_LIST,
      template<typename> typename TRANSFORM,
      template<typename ...> typename RESULT,
      typename = void,
      typename ... Ts>
   struct Transform {
      using type = RESULT<Ts...>;
   };

   template<
      typename INPUT_LIST,
      template<typename> typename TRANSFORM,
      template<typename ...> typename RESULT,
      typename ... Ts>
   struct Transform<INPUT_LIST, TRANSFORM, RESULT, std::void_t<typename INPUT_LIST::Head>, Ts...> {
      using type =
         typename Transform<
            typename INPUT_LIST::Tail,
            TRANSFORM,
            RESULT,
            void,
            Ts..., typename TRANSFORM<typename INPUT_LIST::Head>::type
         >::type;
   };
}

///////////////////////////////////////////////////////////////////////////////////////
template<
   template<typename>     typename TRANSFORMER,
   template<typename ...> typename RESULT,
   typename                    ... Ts>
using Transform_t = typename details::Transform<TypeList < Ts...>, TRANSFORMER, RESULT, void>::type;

///////////////////////////////////////////////////////////////////////////////////////
template<
   template<typename>     typename TRANSFORMER,
   template<typename ...> typename RESULT,
   typename                    ... Ts>
using Transform_tt = typename Transform_t<TRANSFORMER, RESULT, Ts...>::type;

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTTRANSFORM_H
