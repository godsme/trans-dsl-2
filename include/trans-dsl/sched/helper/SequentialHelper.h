//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_SEQUENTIALHELPER_H
#define TRANS_DSL_2_SEQUENTIALHELPER_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/action/SchedSequential.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/sched/helper/VolatileSeq.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/helper/InlineSeqHelper.h>

TSL_NS_BEGIN

struct SchedAction;

namespace details {

   template<typename ... T_ACTIONS>
   class Sequential final {
      enum { Num_Of_Actions = sizeof...(T_ACTIONS) };
      static_assert(Num_Of_Actions >= 2, "__sequential must contain at least 2 actions");
      static_assert(Num_Of_Actions <= 50, "too many actions in a __sequential");

      ///////////////////////////////////////////////////////////////////////////////////////////
      template<typename ... Ts>
      using AllActionsSeq = VolatileSeq<SchedAction, Ts...>;
      using CombType = inline_seq::Comb_t<AllActionsSeq, T_ACTIONS...>;
      using Base = typename CombType::type;

   public:
      struct Inner final : SchedSequential, private Base {
         // for thread-resource-transfer
         using ThreadActionCreator = ThreadCreator_t<T_ACTIONS...>;

         // for inline sequential
         template<size_t N>
         using ActionType = typename inline_seq::Extractor<N, T_ACTIONS...>::type;
         constexpr static size_t totalActions = CombType::totalNumOfActions;
      private:
         OVERRIDE(getNumOfActions()->SeqInt) { return Num_Of_Actions; }
         OVERRIDE(getNext(SeqInt seq) -> SchedAction*) { return Base::get(seq); }
      };
   };

   template<typename ... Ts>
   using Sequential_  = typename Sequential<Ts...>::Inner;
}

#define __sequential(...) TSL_NS::details::Sequential_<__VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_SEQUENTIALHELPER_H
