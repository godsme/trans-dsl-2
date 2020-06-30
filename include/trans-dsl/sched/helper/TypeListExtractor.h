//
// Created by Darwin Yuan on 2020/6/28.
//

#ifndef TRANS_DSL_2_TYPELISTEXTRACTOR_H
#define TRANS_DSL_2_TYPELISTEXTRACTOR_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/SeqInt.h>

TSL_NS_BEGIN

namespace details {
   template<
      SeqInt N,
      template<typename ...> typename TRAIT,
      typename ... Ts>
   struct TypeListExtractor;

   template<
      SeqInt N,
      template<typename ...> typename TRAIT,
      typename HEAD,
      typename ... TAIL>
   struct TypeListExtractor<N, TRAIT, HEAD, TAIL...> {
      using Type = typename TypeListExtractor<SeqInt(N - 1), TRAIT, TAIL...>::Type;
   };

   template<
      template<typename ...> typename TRAIT,
      typename HEAD,
      typename ... TAIL>
   struct TypeListExtractor<0, TRAIT, HEAD, TAIL...> {
      using Type = typename TRAIT<HEAD, TAIL...>::Type;
   };

   template<
      SeqInt N,
      template<typename ...> typename TRAIT,
      typename ... Ts>
    using TypeListExtractor_t = typename TypeListExtractor<N, TRAIT, Ts...>::Type;

   //////////////////////////////////////////////////
   template<typename ...> struct HeadTraits;

   template<typename HEAD, typename ...TAIL>
   struct HeadTraits<HEAD, TAIL...> final {
      using Type = HEAD;
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_TYPELISTEXTRACTOR_H
