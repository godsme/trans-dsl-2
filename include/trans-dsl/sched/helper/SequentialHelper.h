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
#include <trans-dsl/sched/domain/TransListenerObservedAids.h>
#include <trans-dsl/sched/helper/ActionRealTypeTraits.h>
#include <cub/type-list/TypeListPipeLine.h>
#include <trans-dsl/utils/ThreadActionTrait.h>

TSL_NS_BEGIN

struct SchedAction;

namespace details {

   template<typename ... T_ACTIONS>
   class Sequential final {
      enum { Num_Of_Actions = sizeof...(T_ACTIONS) };
      static_assert(Num_Of_Actions >= 2, "__sequential must contain at least 2 actions");
      static_assert(Num_Of_Actions <= 50, "too many actions in a __sequential");

      ///////////////////////////////////////////////////////////////////////////////////////////
      template<TransListenerObservedAids const& AIDs>
      class Trait {
         template<typename T>
         using ToActionRealType = ActionRealTypeTraits<AIDs, T>;

         template<typename ... Ts>
         struct RealBase : CUB_NS::Flattenable<Ts...>, protected VolatileSeq<SchedAction, Ts...> {
            // for thread-resource-transfer
            using ThreadActionCreator = ThreadCreator_t<Ts...>;
         };

      public:
         using Base = __TL_Raw_Pipeline__(T_ACTIONS..., Transform<ToActionRealType>, Flatten)
                      __TL_OutputTo__(RealBase);
      };

   public:
      template<TransListenerObservedAids const& AIDs>
      class ActionRealType : public SchedSequential, public Trait<AIDs>::Base {
         OVERRIDE(getNumOfActions()->SeqInt) { return Num_Of_Actions; }
         OVERRIDE(getNext(SeqInt seq) -> SchedAction*) { return Trait<AIDs>::Base::get(seq); }
      };
   };

   template<typename ... Ts>
   using Sequential_t = typename Sequential<Ts...>::template ActionRealType<EmptyAids>;
}

#define __sequential(...) TSL_NS::details::Sequential<__VA_ARGS__>
#define __def_sequential(...) TSL_NS::details::Sequential_t<__VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_SEQUENTIALHELPER_H
