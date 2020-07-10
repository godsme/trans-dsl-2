//
// Created by Darwin Yuan on 2020/7/10.
//

#ifndef TRANS_DSL_2_INLINESEQHELPER_H
#define TRANS_DSL_2_INLINESEQHELPER_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/tsl_ns.h>
#include <type_traits>
#include <trans-dsl/sched/action/SchedSequential.h>
#include <cub/type-list/TypeListComposer.h>
#include <cub/type-list/TypeListFold.h>

TSL_NS_BEGIN

namespace details {
   class InlineSeq final {
      template<typename COMPOSER, typename T, typename = void>
      struct Seq_ {
         using type = typename COMPOSER::template type<T>;
      };

      template<typename COMPOSER, typename T>
      struct Seq_<COMPOSER, T, std::enable_if_t < std::is_base_of_v < SchedSequential, T>>> {
         using type = typename T::template AllSeq<COMPOSER::template type>;
      };

      template<typename COMPOSER, typename T>
      using Seq = Seq_<COMPOSER, T, void>;

   public:
      template<template<typename ...> typename RESULT, typename ... Ts>
      using type = typename CUB_NS::FoldL_Init_t<Seq, CUB_NS::Composer<>, Ts...>::template output<RESULT>;
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_INLINESEQHELPER_H
