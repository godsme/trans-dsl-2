//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_SWITCHCASEHELPER_H
#define TRANS_DSL_2_SWITCHCASEHELPER_H

#include <trans-dsl/sched/action/SchedSwitchCase.h>
#include <trans-dsl/sched/action/ActionPath.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <cub/base/IsClass.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/sched/helper/Pred.h>
#include <trans-dsl/sched/helper/MaxSizeCalc.h>
#include <trans-dsl/sched/helper/TypeExtractor.h>

TSL_NS_BEGIN

namespace details {

   template<typename T_PRED, typename T_ACTION VOID_CONCEPT>
   struct GenericActionPathClass;

   template<
      typename T_PRED,
      CONCEPT_C(SchedActionConcept, T_ACTION)>
   struct GenericActionPathClass<
      T_PRED,
      T_ACTION
      ENABLE_C(SchedActionConcept, T_ACTION)> : ActionPath {

      OVERRIDE(shouldExecute(TransactionInfo const& trans) -> bool) {
         auto pred = new (cache) T_PRED;
         return (*pred)(trans);
      }

      OVERRIDE(getAction() -> SchedAction&) {
         auto action = new (cache) T_ACTION;
         return *action;
      }

   private:
      alignas(std::max(alignof(T_PRED), alignof(T_ACTION)))
      char cache[std::max(sizeof(T_PRED), sizeof(T_ACTION))];
   };

   template<PredFunction V_PRED, typename T_ACTION>
   struct GenericActionPathFunc : ActionPath {
      OVERRIDE(shouldExecute(TransactionInfo const& trans) -> bool) {
         return V_PRED(trans);
      }

      OVERRIDE(getAction() -> SchedAction&) {
         auto action = new (cache) T_ACTION;
         return *action;
      }

   private:
      alignas(T_ACTION) char cache[sizeof(T_ACTION)];
   };

   template<typename T_PRED, typename T_ACTION>
   auto DeduceActionPath() -> GenericActionPathClass<T_PRED, T_ACTION>;

   template<PredFunction V_PRED, typename T_ACTION>
   auto DeduceActionPath() -> GenericActionPathFunc<V_PRED, T_ACTION>;

   //////////////////////////////////////////////////////////////////////////////////////////

   template<typename T>
   DEF_CONCEPT(ActionPathConcept, std::is_base_of_v<ActionPath, T>);


   // 24 bytes
   template<CONCEPT(ActionPathConcept) ... T_PATHS>
   struct SWITCH__ final : SchedSwitchCase {
      static constexpr size_t Num_Of_Paths = sizeof...(T_PATHS);
      static_assert(Num_Of_Paths >= 2, "should have at least 2 paths, or use __optional instead");
      static_assert(Num_Of_Paths <= 20, "too much paths in one ___switch");

      enum {
         Size  = ( MaxSizeCalc{} << ... << sizeof(T_PATHS) ),
         Align = ( MaxSizeCalc{} << ... << alignof(T_PATHS) )
      };
      alignas(Align) char cache[Size];

      template <SeqInt N>
      auto get() -> ActionPath* {
         if constexpr(N < Num_Of_Paths) {
            using Path = TypeExtractor_t<N, Head, T_PATHS...>;
#if !__CONCEPT_ENABLED
            static_assert(ActionPathConcept<Path>);
#endif
            return new (cache) Path;
         } else {
            return nullptr;
         }
      }

   private:
#define AcTiOn_PaTh_GeT_AcTiOn__(n) case n: return get<n>()

      SeqInt i = 0;
      OVERRIDE(getNext() -> ActionPath *) {
         SeqInt seq = i++;

         if constexpr(Num_Of_Paths <= 2) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
            }
         }
         else if constexpr(Num_Of_Paths == 3) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
            }
         }
         else if constexpr(Num_Of_Paths == 4) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
            }
         }
         else if constexpr(Num_Of_Paths == 5) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
            }
         }
         else if constexpr(Num_Of_Paths == 6) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
               AcTiOn_PaTh_GeT_AcTiOn__(5);
            }
         }
         else if constexpr(Num_Of_Paths == 7) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
               AcTiOn_PaTh_GeT_AcTiOn__(5);
               AcTiOn_PaTh_GeT_AcTiOn__(6);
            }
         }
         else if constexpr(Num_Of_Paths == 8) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
               AcTiOn_PaTh_GeT_AcTiOn__(5);
               AcTiOn_PaTh_GeT_AcTiOn__(6);
               AcTiOn_PaTh_GeT_AcTiOn__(7);
            }
         }
         else if constexpr(Num_Of_Paths == 9) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
               AcTiOn_PaTh_GeT_AcTiOn__(5);
               AcTiOn_PaTh_GeT_AcTiOn__(6);
               AcTiOn_PaTh_GeT_AcTiOn__(7);
               AcTiOn_PaTh_GeT_AcTiOn__(8);
            }
         }
         else if constexpr(Num_Of_Paths == 10) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
               AcTiOn_PaTh_GeT_AcTiOn__(5);
               AcTiOn_PaTh_GeT_AcTiOn__(6);
               AcTiOn_PaTh_GeT_AcTiOn__(7);
               AcTiOn_PaTh_GeT_AcTiOn__(8);
               AcTiOn_PaTh_GeT_AcTiOn__(9);
            }
         }
         else if constexpr(Num_Of_Paths == 11) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
               AcTiOn_PaTh_GeT_AcTiOn__(5);
               AcTiOn_PaTh_GeT_AcTiOn__(6);
               AcTiOn_PaTh_GeT_AcTiOn__(7);
               AcTiOn_PaTh_GeT_AcTiOn__(8);
               AcTiOn_PaTh_GeT_AcTiOn__(9);
               AcTiOn_PaTh_GeT_AcTiOn__(10);
            }
         }
         else if constexpr(Num_Of_Paths == 12) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
               AcTiOn_PaTh_GeT_AcTiOn__(5);
               AcTiOn_PaTh_GeT_AcTiOn__(6);
               AcTiOn_PaTh_GeT_AcTiOn__(7);
               AcTiOn_PaTh_GeT_AcTiOn__(8);
               AcTiOn_PaTh_GeT_AcTiOn__(9);
               AcTiOn_PaTh_GeT_AcTiOn__(10);
               AcTiOn_PaTh_GeT_AcTiOn__(11);
            }
         }
         else if constexpr(Num_Of_Paths == 13) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
               AcTiOn_PaTh_GeT_AcTiOn__(5);
               AcTiOn_PaTh_GeT_AcTiOn__(6);
               AcTiOn_PaTh_GeT_AcTiOn__(7);
               AcTiOn_PaTh_GeT_AcTiOn__(8);
               AcTiOn_PaTh_GeT_AcTiOn__(9);
               AcTiOn_PaTh_GeT_AcTiOn__(10);
               AcTiOn_PaTh_GeT_AcTiOn__(11);
               AcTiOn_PaTh_GeT_AcTiOn__(12);
            }
         }
         else if constexpr(Num_Of_Paths == 14) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
               AcTiOn_PaTh_GeT_AcTiOn__(5);
               AcTiOn_PaTh_GeT_AcTiOn__(6);
               AcTiOn_PaTh_GeT_AcTiOn__(7);
               AcTiOn_PaTh_GeT_AcTiOn__(8);
               AcTiOn_PaTh_GeT_AcTiOn__(9);
               AcTiOn_PaTh_GeT_AcTiOn__(10);
               AcTiOn_PaTh_GeT_AcTiOn__(11);
               AcTiOn_PaTh_GeT_AcTiOn__(12);
               AcTiOn_PaTh_GeT_AcTiOn__(13);
            }
         }
         else if constexpr(Num_Of_Paths == 15) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
               AcTiOn_PaTh_GeT_AcTiOn__(5);
               AcTiOn_PaTh_GeT_AcTiOn__(6);
               AcTiOn_PaTh_GeT_AcTiOn__(7);
               AcTiOn_PaTh_GeT_AcTiOn__(8);
               AcTiOn_PaTh_GeT_AcTiOn__(9);
               AcTiOn_PaTh_GeT_AcTiOn__(10);
               AcTiOn_PaTh_GeT_AcTiOn__(11);
               AcTiOn_PaTh_GeT_AcTiOn__(12);
               AcTiOn_PaTh_GeT_AcTiOn__(13);
               AcTiOn_PaTh_GeT_AcTiOn__(14);
            }
         }
         else if constexpr(Num_Of_Paths == 16) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
               AcTiOn_PaTh_GeT_AcTiOn__(5);
               AcTiOn_PaTh_GeT_AcTiOn__(6);
               AcTiOn_PaTh_GeT_AcTiOn__(7);
               AcTiOn_PaTh_GeT_AcTiOn__(8);
               AcTiOn_PaTh_GeT_AcTiOn__(9);
               AcTiOn_PaTh_GeT_AcTiOn__(10);
               AcTiOn_PaTh_GeT_AcTiOn__(11);
               AcTiOn_PaTh_GeT_AcTiOn__(12);
               AcTiOn_PaTh_GeT_AcTiOn__(13);
               AcTiOn_PaTh_GeT_AcTiOn__(14);
               AcTiOn_PaTh_GeT_AcTiOn__(15);
            }
         }
         else if constexpr(Num_Of_Paths == 17) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
               AcTiOn_PaTh_GeT_AcTiOn__(5);
               AcTiOn_PaTh_GeT_AcTiOn__(6);
               AcTiOn_PaTh_GeT_AcTiOn__(7);
               AcTiOn_PaTh_GeT_AcTiOn__(8);
               AcTiOn_PaTh_GeT_AcTiOn__(9);
               AcTiOn_PaTh_GeT_AcTiOn__(10);
               AcTiOn_PaTh_GeT_AcTiOn__(11);
               AcTiOn_PaTh_GeT_AcTiOn__(12);
               AcTiOn_PaTh_GeT_AcTiOn__(13);
               AcTiOn_PaTh_GeT_AcTiOn__(14);
               AcTiOn_PaTh_GeT_AcTiOn__(15);
               AcTiOn_PaTh_GeT_AcTiOn__(16);
            }
         }
         else if constexpr(Num_Of_Paths == 18) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
               AcTiOn_PaTh_GeT_AcTiOn__(5);
               AcTiOn_PaTh_GeT_AcTiOn__(6);
               AcTiOn_PaTh_GeT_AcTiOn__(7);
               AcTiOn_PaTh_GeT_AcTiOn__(8);
               AcTiOn_PaTh_GeT_AcTiOn__(9);
               AcTiOn_PaTh_GeT_AcTiOn__(10);
               AcTiOn_PaTh_GeT_AcTiOn__(11);
               AcTiOn_PaTh_GeT_AcTiOn__(12);
               AcTiOn_PaTh_GeT_AcTiOn__(13);
               AcTiOn_PaTh_GeT_AcTiOn__(14);
               AcTiOn_PaTh_GeT_AcTiOn__(15);
               AcTiOn_PaTh_GeT_AcTiOn__(16);
               AcTiOn_PaTh_GeT_AcTiOn__(17);
            }
         }
         else if constexpr(Num_Of_Paths == 19) {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
               AcTiOn_PaTh_GeT_AcTiOn__(5);
               AcTiOn_PaTh_GeT_AcTiOn__(6);
               AcTiOn_PaTh_GeT_AcTiOn__(7);
               AcTiOn_PaTh_GeT_AcTiOn__(8);
               AcTiOn_PaTh_GeT_AcTiOn__(9);
               AcTiOn_PaTh_GeT_AcTiOn__(10);
               AcTiOn_PaTh_GeT_AcTiOn__(11);
               AcTiOn_PaTh_GeT_AcTiOn__(12);
               AcTiOn_PaTh_GeT_AcTiOn__(13);
               AcTiOn_PaTh_GeT_AcTiOn__(14);
               AcTiOn_PaTh_GeT_AcTiOn__(15);
               AcTiOn_PaTh_GeT_AcTiOn__(16);
               AcTiOn_PaTh_GeT_AcTiOn__(17);
               AcTiOn_PaTh_GeT_AcTiOn__(18);
            }
         }
         else {
            switch (seq) {
               AcTiOn_PaTh_GeT_AcTiOn__(0);
               AcTiOn_PaTh_GeT_AcTiOn__(1);
               AcTiOn_PaTh_GeT_AcTiOn__(2);
               AcTiOn_PaTh_GeT_AcTiOn__(3);
               AcTiOn_PaTh_GeT_AcTiOn__(4);
               AcTiOn_PaTh_GeT_AcTiOn__(5);
               AcTiOn_PaTh_GeT_AcTiOn__(6);
               AcTiOn_PaTh_GeT_AcTiOn__(7);
               AcTiOn_PaTh_GeT_AcTiOn__(8);
               AcTiOn_PaTh_GeT_AcTiOn__(9);
               AcTiOn_PaTh_GeT_AcTiOn__(10);
               AcTiOn_PaTh_GeT_AcTiOn__(11);
               AcTiOn_PaTh_GeT_AcTiOn__(12);
               AcTiOn_PaTh_GeT_AcTiOn__(13);
               AcTiOn_PaTh_GeT_AcTiOn__(14);
               AcTiOn_PaTh_GeT_AcTiOn__(15);
               AcTiOn_PaTh_GeT_AcTiOn__(16);
               AcTiOn_PaTh_GeT_AcTiOn__(17);
               AcTiOn_PaTh_GeT_AcTiOn__(18);
               AcTiOn_PaTh_GeT_AcTiOn__(19);
            }
         }
         return nullptr;
      }
   };

   inline auto IsTrue__(const TransactionInfo&) -> bool { return true; }
}

TSL_NS_END

#define __case(pred, action) decltype(TSL_NS::details::DeduceActionPath<pred, action>())
#define __otherwise(action) __case(TSL_NS::details::IsTrue__, action)
#define __switch(...) TSL_NS::details::SWITCH__<__VA_ARGS__>

#endif //TRANS_DSL_2_SWITCHCASEHELPER_H
