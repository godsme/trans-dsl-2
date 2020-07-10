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
   template<typename ... T_PATHS>
   class Switch final  {
      static constexpr size_t Num_Of_Paths = sizeof...(T_PATHS);
      static_assert(Num_Of_Paths >= 2, "should have at least 2 paths, or use __optional instead");
      static_assert(Num_Of_Paths <= 20, "too much paths in one ___switch");

      template<TransListenerObservedAids const& AIDs>
      struct Trait {
         template<typename ... Tss>
         struct RealTypes  {
            // for thread-resource-transfer
            using ThreadActionCreator = ThreadCreator_t<Tss...>;
            using Paths = VolatileSeq<ActionPath, Tss...>;
         };

         template<typename T>
         using ToRealTypes = ActionRealTypeTraits<AIDs, T, void>;

         using type = CUB_NS::Transform_t<ToRealTypes, RealTypes, T_PATHS...>;
      };

   public:
      template<TransListenerObservedAids const& AIDs>
      struct ActionRealType : SchedSwitchCase, Trait<AIDs>::type::Paths {
         using ThreadActionCreator = typename Trait<AIDs>::type::ThreadActionCreator;
      private:
         SeqInt i = 0;
         OVERRIDE(getNext()->ActionPath *) {
            return Trait<AIDs>::type::Paths::get(i++);
         }
      };
   };

   template<typename ... Ts>
   using Switch_t = typename Switch<Ts...>::template ActionRealType<EmptyAids>;
}

TSL_NS_END

#define __switch(...) TSL_NS::details::Switch<__VA_ARGS__>
#define __def_switch(...) TSL_NS::details::Switch_t<__VA_ARGS__>

#endif //TRANS_DSL_2_SWITCHCASEHELPER_H
