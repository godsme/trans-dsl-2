//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_SEQUENTIALHELPER_H
#define TRANS_DSL_2_SEQUENTIALHELPER_H

#include <trans-dsl/tsl_ns.h>
#include <cstddef>
#include <algorithm>
#include <trans-dsl/sched/action/SchedSequential.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>

TSL_NS_BEGIN

struct SchedAction;

template<size_t T_SIZE, size_t T_ALIGN, SeqInt T_SEQ  VOID_CONCEPT, typename ... T_ACTIONS>
struct GenericSequential;

template<
   size_t T_SIZE,
   size_t T_ALIGN,
   SeqInt T_SEQ,
   typename T_HEAD,
   typename ... T_TAIL>
struct GenericSequential<
   T_SIZE,
   T_ALIGN,
   T_SEQ
   ENABLE_C(SchedActionConcept, T_HEAD),
   T_HEAD,
   T_TAIL...> {
   using Action = T_HEAD;
   using Next =
      typename GenericSequential<
         std::max(T_SIZE, sizeof(Action)),
         std::max(T_ALIGN, alignof(Action)),
         T_SEQ + 1
         VOID_PLACEHOLDER,
         T_TAIL...>::Inner;

   struct Inner : Next {
      using Next::cache;

      auto get(SeqInt seq) -> SchedAction* {
         return seq == T_SEQ ? new (cache) Action : Next::get(seq);
      }
   };
};

template<size_t T_SIZE, size_t T_ALIGN, SeqInt T_SEQ>
struct GenericSequential<T_SIZE, T_ALIGN, T_SEQ> {
   struct Inner  {
      auto get(SeqInt seq) -> SchedAction* {
         return nullptr;
      }
   protected:
      alignas(T_ALIGN) char cache[T_SIZE];
   };
};

template<CONCEPT(SchedActionConcept) ... T_ACTIONS>
struct SEQUENTIAL__ {
   enum { NUM_OF_ACTIONS = sizeof...(T_ACTIONS) };
   static_assert(NUM_OF_ACTIONS >=2, "__sequential must contain at least 2 actions");
   static_assert(NUM_OF_ACTIONS <= std::numeric_limits<SeqInt>::max(), "too many actions in a __sequential");

   using Actions = typename GenericSequential<0, 0, 0 VOID_PLACEHOLDER, T_ACTIONS...>::Inner;
   struct Inner : SchedSequential, private Actions {
   private:
      OVERRIDE(getNumOfActions() -> SeqInt) {
         return NUM_OF_ACTIONS;
      }
      OVERRIDE(getNext(SeqInt index) -> SchedAction*) {
         return Actions::get(index);
      }
   };
};


#define __sequential(...) typename TSL_NS::SEQUENTIAL__<__VA_ARGS__>::Inner

TSL_NS_END

#endif //TRANS_DSL_2_SEQUENTIALHELPER_H
