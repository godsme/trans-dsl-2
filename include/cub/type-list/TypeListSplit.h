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
      using type = typename Split<
         typename IN::Tail,
         N - 1,
         __TYPE_LIST_APPEND(OUT..., typename IN::Head)
      >::type;
   };

   template<
      typename IN,
      typename ... OUT>
   struct Split<IN, 0, OUT...> {
      using type = __TL_make_pair(TypeList<OUT...>, IN);
   };
}

namespace details {
   template<
      typename IN,
      size_t N,
      typename ... OUT>
   struct Take {
      using type = typename Take<
         typename IN::Tail,
         N - 1,
         __TYPE_LIST_APPEND(OUT..., typename IN::Head)
      >::type;
   };

   template<
      typename IN,
      typename ... OUT>
   struct Take<IN, 0,  OUT...> {
      using type = TypeList<OUT...>;
   };
}

namespace type_list {
   template<
      typename       IN,
      size_t         N>
   using Split_t = typename details::Split<IN, N>::type;

   //////////////////////////////////////////////////////////////////////
   template<
      typename       IN,
      size_t         N>
   using Take_t = typename details::Take<IN, N>::type;
}

//////////////////////////////////////////////////////////////////////
namespace details {
   template<
      typename IN,
      size_t N,
      template<typename ...> typename RESULT_1,
      template<typename ...> typename RESULT_2>
   struct Splitter {
      using RawType = type_list::Split_t<IN, N>;
      using type = __TL_make_pair(typename RawType::first::template output<RESULT_1>,
                                  typename RawType::second::template output<RESULT_2>);
   };
}
template<
   size_t N,
   template<typename ...> typename RESULT_1,
   template<typename ...> typename RESULT_2,
   typename ... IN>
using Split_t = typename details::Splitter<TypeList<IN...>, N, RESULT_1, RESULT_2>::type;

//////////////////////////////////////////////////////////////////////
template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using Take_t = typename type_list::Take_t<TypeList<IN...>, N>::template output<RESULT>;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using Take_tt = typename Take_t<N, RESULT, IN...>::type;

//////////////////////////////////////////////////////////////////////
namespace details {
   template<
      typename IN,
      size_t N>
   struct DropRight {
      static_assert(N >= IN::size, "N is greater than the size of type list");
      using type = type_list::Take_t<IN, IN::size - N>;
   };
}

namespace type_list {
   template<
      typename        IN,
      size_t          N>
   using DropRight_t = typename details::DropRight<IN, N>::type;
}

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using DropRight_t = typename type_list::DropRight_t<TypeList<IN...>, N>::template output<RESULT>;

template<
   size_t N,
   template<typename ...> typename RESULT,
   typename ... IN>
using DropRight_tt = typename DropRight_t<N, RESULT, IN...>::type;

CUB_NS_END

#endif //TRANS_DSL_2_TYPELISTSPLIT_H
