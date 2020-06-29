//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_LOOPHELPER_H
#define TRANS_DSL_2_LOOPHELPER_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/sched/helper/LoopPred.h>
#include <trans-dsl/sched/helper/IsSchedAction.h>
#include <trans-dsl/sched/helper/LoopPredAction.h>
#include <trans-dsl/sched/action/SchedLoop.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/sched/concepts/ConceptHelper.h>
#include <trans-dsl/sched/helper/MaxSizeCalc.h>
#include <trans-dsl/sched/helper/TypeExtractor.h>

TSL_NS_BEGIN

namespace details {

   template<VOID_CONCEPT_2 typename ... T_ACTIONS>
   struct GenericLoop_;

   ///////////////////////////////////////////////////////////////
   template<
      typename T_HEAD,
      typename ... T_TAIL>
   struct GenericLoopPred : GenericLoop_< VOID_PLACEHOLDER_2 T_TAIL...> {
      auto get(char*, bool& isAction) -> SchedAction* {
            isAction = false;
            return &pred;
      }
   private:
      GenericLoopAction<T_HEAD> pred;
   };

   ///////////////////////////////////////////////////////////////////////////////////////
   template<
      template<typename T> typename T_TRAITS,
      typename T_HEAD,
      typename ... T_TAIL>
   struct GenericLoopEmpty_ : GenericLoop_<VOID_PLACEHOLDER_2 T_TAIL...> {
      auto get(char* cache, bool& isAction) -> SchedAction* {
         isAction = T_TRAITS<T_HEAD>::isAction;
         using Action = typename T_TRAITS<T_HEAD>::Action;
         return new (cache) Action;
      }
   };

   /////////////////////////////////////////////////////////////////////////////////////////
   template<typename T_PRED>
   struct LoopPredTraits {
      using Action = GenericLoopAction<T_PRED>;
      constexpr static bool isAction = false;
   };

   template<typename T>
   DEF_CONCEPT(LoopPredConcept, std::is_base_of_v<LoopPredSignature, T>);

   template<typename T>
   DEF_CONCEPT(NonEmptyLoopPredConcept, LoopPredConcept<T> && (sizeof(T) > 1));

   template<typename T>
   DEF_CONCEPT(EmptyLoopPredConcept, LoopPredConcept<T> && (sizeof(T) == 1));

   /////////////////////////////////////////////////////////////////////////////////////////
   template<
      CONCEPT_C(NonEmptyLoopPredConcept, T_HEAD),
      typename ... T_TAIL>
   struct GenericLoop_<
      ENABLE_C_2(NonEmptyLoopPredConcept, T_HEAD)
      T_HEAD,
      T_TAIL...>
      : GenericLoopPred<T_HEAD, T_TAIL...>
   {};

   ///////////////////////////////////////////////////////////////////////////////////////

   template<
      CONCEPT_C(EmptyLoopPredConcept, T_HEAD),
      typename ... T_TAIL>
   struct GenericLoop_<
      ENABLE_C_2(EmptyLoopPredConcept, T_HEAD)
      T_HEAD,
      T_TAIL...>
      : GenericLoopEmpty_<LoopPredTraits, T_HEAD, T_TAIL...>
   {};

   ///////////////////////////////////////////////////////////////////////////////////////
   template<typename T>
   struct ActionTraits {
      using Action = T;
      constexpr static bool isAction = true;
   };

   template<
      CONCEPT_C(SchedActionConcept, T_HEAD),
      typename ... T_TAIL>
   struct GenericLoop_<
      ENABLE_C_2(SchedActionConcept, T_HEAD)
      T_HEAD,
      T_TAIL...>
      : GenericLoopEmpty_<ActionTraits, T_HEAD, T_TAIL...>{};

   /////////////////////////////////////////////////////////////////////////////////////////////
   template<>
   struct GenericLoop_<> {
      struct Inner  {
         auto get(char*, bool&) -> SchedAction* {
            return nullptr;
         }
      };
   };

   template<typename T>
   DEF_CONCEPT(LoopElemConcept, LoopPredConcept<T> || SchedActionConcept<T>);

   template<uint32_t V_MAX_TIMES, CONCEPT(LoopElemConcept) ... T_ACTIONS>
   struct Loop final : private GenericLoop_<VOID_PLACEHOLDER_2 T_ACTIONS...>, SchedLoop {
   private:
      enum { Num_Of_Actions = sizeof...(T_ACTIONS) };
      static_assert(Num_Of_Actions > 0, "loop cannot be empty");
      static_assert(Num_Of_Actions <= 20, "too many actions in a loop");

   private:
      template <typename T>
      constexpr static size_t Size_Of  = SchedActionConcept<T> ? sizeof(T) : 0 ;

      template <typename T>
      constexpr static size_t Align_Of = SchedActionConcept<T> ? alignof(T) : 0 ;

      enum {
         Size  = ( MaxSizeCalc{} << ... << Size_Of<T_ACTIONS> ),
         Align = ( MaxSizeCalc{} << ... << Align_Of<T_ACTIONS> )
      };
      alignas(Align) char cache[Size];

      template<typename ... Ts>
      struct LoopElem {
         using Type = GenericLoop_<VOID_PLACEHOLDER_2 Ts...>;
      };

      template <SeqInt N>
      auto get(bool& isAction) -> SchedAction* {
         if constexpr(N < Num_Of_Actions) {
            return TypeExtractor_t<N, LoopElem, T_ACTIONS...>::get(cache, isAction);
         } else {
            return nullptr;
         }
      }

   public:
      OVERRIDE(getMaxTime() const -> uint32_t) {
         return V_MAX_TIMES;
      }

#define LoOp_GeT_AcTiOn__(n) case n: return get<n>(isAction)

      OVERRIDE(getAction(SeqInt seq, bool& isAction) -> SchedAction*) {
         if constexpr(Num_Of_Actions <= 2) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
            }
         }
         else if constexpr(Num_Of_Actions == 3) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
            }
         }
         else if constexpr(Num_Of_Actions == 4) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
            }
         }
         else if constexpr(Num_Of_Actions == 5) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
            }
         }
         else if constexpr(Num_Of_Actions == 6) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
               LoOp_GeT_AcTiOn__(5);
            }
         }
         else if constexpr(Num_Of_Actions == 7) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
               LoOp_GeT_AcTiOn__(5);
               LoOp_GeT_AcTiOn__(6);
            }
         }
         else if constexpr(Num_Of_Actions == 8) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
               LoOp_GeT_AcTiOn__(5);
               LoOp_GeT_AcTiOn__(6);
               LoOp_GeT_AcTiOn__(7);
            }
         }
         else if constexpr(Num_Of_Actions == 9) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
               LoOp_GeT_AcTiOn__(5);
               LoOp_GeT_AcTiOn__(6);
               LoOp_GeT_AcTiOn__(7);
               LoOp_GeT_AcTiOn__(8);
            }
         }
         else if constexpr(Num_Of_Actions == 10) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
               LoOp_GeT_AcTiOn__(5);
               LoOp_GeT_AcTiOn__(6);
               LoOp_GeT_AcTiOn__(7);
               LoOp_GeT_AcTiOn__(8);
               LoOp_GeT_AcTiOn__(9);
            }
         }
         else if constexpr(Num_Of_Actions == 11) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
               LoOp_GeT_AcTiOn__(5);
               LoOp_GeT_AcTiOn__(6);
               LoOp_GeT_AcTiOn__(7);
               LoOp_GeT_AcTiOn__(8);
               LoOp_GeT_AcTiOn__(9);
               LoOp_GeT_AcTiOn__(10);
            }
         }
         else if constexpr(Num_Of_Actions == 12) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
               LoOp_GeT_AcTiOn__(5);
               LoOp_GeT_AcTiOn__(6);
               LoOp_GeT_AcTiOn__(7);
               LoOp_GeT_AcTiOn__(8);
               LoOp_GeT_AcTiOn__(9);
               LoOp_GeT_AcTiOn__(10);
               LoOp_GeT_AcTiOn__(11);
            }
         }
         else if constexpr(Num_Of_Actions == 13) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
               LoOp_GeT_AcTiOn__(5);
               LoOp_GeT_AcTiOn__(6);
               LoOp_GeT_AcTiOn__(7);
               LoOp_GeT_AcTiOn__(8);
               LoOp_GeT_AcTiOn__(9);
               LoOp_GeT_AcTiOn__(10);
               LoOp_GeT_AcTiOn__(11);
               LoOp_GeT_AcTiOn__(12);
            }
         }
         else if constexpr(Num_Of_Actions == 14) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
               LoOp_GeT_AcTiOn__(5);
               LoOp_GeT_AcTiOn__(6);
               LoOp_GeT_AcTiOn__(7);
               LoOp_GeT_AcTiOn__(8);
               LoOp_GeT_AcTiOn__(9);
               LoOp_GeT_AcTiOn__(10);
               LoOp_GeT_AcTiOn__(11);
               LoOp_GeT_AcTiOn__(12);
               LoOp_GeT_AcTiOn__(13);
            }
         }
         else if constexpr(Num_Of_Actions == 15) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
               LoOp_GeT_AcTiOn__(5);
               LoOp_GeT_AcTiOn__(6);
               LoOp_GeT_AcTiOn__(7);
               LoOp_GeT_AcTiOn__(8);
               LoOp_GeT_AcTiOn__(9);
               LoOp_GeT_AcTiOn__(10);
               LoOp_GeT_AcTiOn__(11);
               LoOp_GeT_AcTiOn__(12);
               LoOp_GeT_AcTiOn__(13);
               LoOp_GeT_AcTiOn__(14);
            }
         }
         else if constexpr(Num_Of_Actions == 16) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
               LoOp_GeT_AcTiOn__(5);
               LoOp_GeT_AcTiOn__(6);
               LoOp_GeT_AcTiOn__(7);
               LoOp_GeT_AcTiOn__(8);
               LoOp_GeT_AcTiOn__(9);
               LoOp_GeT_AcTiOn__(10);
               LoOp_GeT_AcTiOn__(11);
               LoOp_GeT_AcTiOn__(12);
               LoOp_GeT_AcTiOn__(13);
               LoOp_GeT_AcTiOn__(14);
               LoOp_GeT_AcTiOn__(15);
            }
         }
         else if constexpr(Num_Of_Actions == 17) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
               LoOp_GeT_AcTiOn__(5);
               LoOp_GeT_AcTiOn__(6);
               LoOp_GeT_AcTiOn__(7);
               LoOp_GeT_AcTiOn__(8);
               LoOp_GeT_AcTiOn__(9);
               LoOp_GeT_AcTiOn__(10);
               LoOp_GeT_AcTiOn__(11);
               LoOp_GeT_AcTiOn__(12);
               LoOp_GeT_AcTiOn__(13);
               LoOp_GeT_AcTiOn__(14);
               LoOp_GeT_AcTiOn__(15);
               LoOp_GeT_AcTiOn__(16);
            }
         }
         else if constexpr(Num_Of_Actions == 18) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
               LoOp_GeT_AcTiOn__(5);
               LoOp_GeT_AcTiOn__(6);
               LoOp_GeT_AcTiOn__(7);
               LoOp_GeT_AcTiOn__(8);
               LoOp_GeT_AcTiOn__(9);
               LoOp_GeT_AcTiOn__(10);
               LoOp_GeT_AcTiOn__(11);
               LoOp_GeT_AcTiOn__(12);
               LoOp_GeT_AcTiOn__(13);
               LoOp_GeT_AcTiOn__(14);
               LoOp_GeT_AcTiOn__(15);
               LoOp_GeT_AcTiOn__(16);
               LoOp_GeT_AcTiOn__(17);
            }
         }
         else if constexpr(Num_Of_Actions == 19) {
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
               LoOp_GeT_AcTiOn__(5);
               LoOp_GeT_AcTiOn__(6);
               LoOp_GeT_AcTiOn__(7);
               LoOp_GeT_AcTiOn__(8);
               LoOp_GeT_AcTiOn__(9);
               LoOp_GeT_AcTiOn__(10);
               LoOp_GeT_AcTiOn__(11);
               LoOp_GeT_AcTiOn__(12);
               LoOp_GeT_AcTiOn__(13);
               LoOp_GeT_AcTiOn__(14);
               LoOp_GeT_AcTiOn__(15);
               LoOp_GeT_AcTiOn__(16);
               LoOp_GeT_AcTiOn__(17);
               LoOp_GeT_AcTiOn__(18);
            }
         } else{
            switch (seq) {
               LoOp_GeT_AcTiOn__(0);
               LoOp_GeT_AcTiOn__(1);
               LoOp_GeT_AcTiOn__(2);
               LoOp_GeT_AcTiOn__(3);
               LoOp_GeT_AcTiOn__(4);
               LoOp_GeT_AcTiOn__(5);
               LoOp_GeT_AcTiOn__(6);
               LoOp_GeT_AcTiOn__(7);
               LoOp_GeT_AcTiOn__(8);
               LoOp_GeT_AcTiOn__(9);
               LoOp_GeT_AcTiOn__(10);
               LoOp_GeT_AcTiOn__(11);
               LoOp_GeT_AcTiOn__(12);
               LoOp_GeT_AcTiOn__(13);
               LoOp_GeT_AcTiOn__(14);
               LoOp_GeT_AcTiOn__(15);
               LoOp_GeT_AcTiOn__(16);
               LoOp_GeT_AcTiOn__(17);
               LoOp_GeT_AcTiOn__(18);
               LoOp_GeT_AcTiOn__(19);
            }
         }
         return nullptr;
      }
   };
}

#define __loop(...) TSL_NS::details::Loop<1, __VA_ARGS__>
#define __loop_max(times, ...) TSL_NS::details::LOOP__<times, __VA_ARGS__>::Inner
#define __forever(...) TSL_NS::details::LOOP__<std::numeric_limits<uint32>::max(), __VA_ARGS__>::Inner

#define __break_if(pred, ...) decltype(TSL_NS::details::DeduceLoopPredType<pred, ##__VA_ARGS__>())
#define __until(...) __break_if(__VA_ARGS__)
#define __redo_if(pred) decltype(TSL_NS::details::DeduceLoopPredType<pred, TSL_NS::Result::RESTART_REQUIRED>())
#define __while(pred, ...) __break_if(__not(pred), ##__VA_ARGS__)

TSL_NS_END

#endif //TRANS_DSL_2_LOOPHELPER_H
