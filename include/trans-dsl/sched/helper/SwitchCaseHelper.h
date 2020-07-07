//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_SWITCHCASEHELPER_H
#define TRANS_DSL_2_SWITCHCASEHELPER_H

#include <trans-dsl/sched/action/SchedSwitchCase.h>
#include <trans-dsl/sched/helper/ActionPathHelper.h>
#include <trans-dsl/sched/helper/VolatileSeq.h>
#include <trans-dsl/utils/ThreadActionTrait.h>

TSL_NS_BEGIN

namespace details {

   template<typename T>
   DEF_CONCEPT(ActionPathConcept, std::is_base_of_v<ActionPath, T>);

   // 24 bytes
   template<CONCEPT(ActionPathConcept) ... T_PATHS>
   class Switch final  {
      static constexpr size_t Num_Of_Paths = sizeof...(T_PATHS);
      static_assert(Num_Of_Paths >= 2, "should have at least 2 paths, or use __optional instead");
      static_assert(Num_Of_Paths <= 20, "too much paths in one ___switch");

      using Base = VolatileSeq<ActionPath, T_PATHS...>;

   public:
      struct Inner : SchedSwitchCase, private Base {
         using ThreadActionCreator = ThreadCreator_t<T_PATHS...>;
      private:
         SeqInt i = 0;
         OVERRIDE(getNext()->ActionPath *) {
            return Base::get(i++);
         }
      };
   };
}

TSL_NS_END

#define __switch(...) typename TSL_NS::details::Switch<__VA_ARGS__>::Inner

#endif //TRANS_DSL_2_SWITCHCASEHELPER_H
