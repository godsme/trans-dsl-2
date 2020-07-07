//
// Created by Darwin Yuan on 2020/7/7.
//

#ifndef TRANS_DSL_2_TYPELISTTRANSFORMER_H
#define TRANS_DSL_2_TYPELISTTRANSFORMER_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/SeqInt.h>
#include <cstddef>
#include "TypeListExtractor.h"

TSL_NS_BEGIN

namespace details :: transformer {
   template<typename ... Ts>
   struct TypeN {
      template<size_t N>
      using elem = TypeListExtractor_t<N, HeadTraits, Ts...>;
   };
}

namespace details {
   template<
      size_t N,
      size_t M,
      template<typename> typename TRANSFORM,
      template<typename ...> typename RESULT,
      typename EXTRACTOR,
      typename ... Ts>
   struct Transformer {
   private:
      using Elem = typename EXTRACTOR::template elem<M>;
   public:
      using type = typename Transformer<N-1, M+1, TRANSFORM, RESULT, EXTRACTOR, Ts..., typename TRANSFORM<Elem>::type>::type;
   };

   template<size_t M, template<typename> typename TRANSFORM, template<typename ...> typename RESULT, typename EXTRACTOR, typename ... Ts>
   struct Transformer<0, M, TRANSFORM, RESULT, EXTRACTOR, Ts...> {
      using type = RESULT<Ts...>;
   };

   template<
      template<typename> typename TRANSFORMER,
      template<typename ...> typename RESULT,
      typename ... Ts>
   using Transform_t = typename Transformer<sizeof...(Ts), 0, TRANSFORMER, RESULT, transformer::TypeN<Ts...>>::type;

   template<
      template<typename> typename TRANSFORMER,
      template<typename ...> typename RESULT,
      typename ... Ts>
   using Transform_tt = typename Transform_t<TRANSFORMER, RESULT, Ts...>::type;
}

TSL_NS_END

#endif //TRANS_DSL_2_TYPELISTTRANSFORMER_H
