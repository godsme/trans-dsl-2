//
// Created by Darwin Yuan on 2020/6/28.
//

#ifndef TRANS_DSL_2_TYPEEXTRACTOR_H
#define TRANS_DSL_2_TYPEEXTRACTOR_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/SeqInt.h>

TSL_NS_BEGIN

namespace details {
   template<
      SeqInt N,
      template<typename ...> typename TRAIT,
      typename ... Ts>
   struct TypeExtractor;

   template<
      SeqInt N,
      template<typename ...> typename TRAIT,
      typename HEAD,
      typename ... TAIL>
   struct TypeExtractor<N, TRAIT, HEAD, TAIL...> {
      using Type = typename TypeExtractor<N-1, TRAIT, TAIL...>::Type;
   };

   template<
      template<typename ...> typename TRAIT,
      typename HEAD,
      typename ... TAIL>
   struct TypeExtractor<0, TRAIT, HEAD, TAIL...> {
      using Type = typename TRAIT<HEAD, TAIL...>::Type;
   };

   template<
      SeqInt N,
      template<typename ...> typename TRAIT,
      typename ... Ts>
    using TypeExtractor_t = typename TypeExtractor<N, TRAIT, Ts...>::Type;

   //////////////////////////////////////////////////
   template<typename ...> struct Head;

   template<typename HEAD, typename ...TAIL>
   struct Head<HEAD, TAIL...> final {
      using Type = HEAD;
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_TYPEEXTRACTOR_H
