//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_SEQUENTIAL_H
#define TRANS_DSL_2_SEQUENTIAL_H

#include <trans-dsl/tsl_ns.h>
#include <cstddef>
#include <algorithm>
#include <trans-dsl/sched/helper/SchedActionWrapper.h>
#include <trans-dsl/sched/action/SchedSequential.h>

TSL_NS_BEGIN

struct SchedAction;

template<size_t T_SIZE, size_t T_ALIGN, unsigned int T_SEQ, typename ... T_ACTIONS>
struct GenericSequential;

template<size_t T_SIZE, size_t T_ALIGN, unsigned int T_SEQ, typename T_HEAD, typename ... T_TAIL>
struct GenericSequential<T_SIZE, T_ALIGN, T_SEQ, T_HEAD, T_TAIL...> {
   using Action = T_HEAD;
   using Next =
      typename GenericSequential<
         std::max(T_SIZE, sizeof(Action)),
         std::max(T_ALIGN, alignof(Action)),
         T_SEQ + 1,
         T_TAIL...>::Inner;

   struct Inner : Next {
      using Next::cache;

      auto get(unsigned int seq) -> SchedAction* {
         return seq == T_SEQ ? new (cache) Action : Next::get(seq);
      }
   };
};

template<size_t T_SIZE, size_t T_ALIGN, unsigned int T_SEQ>
struct GenericSequential<T_SIZE, T_ALIGN, T_SEQ> {
   struct Inner  {
      auto get(unsigned int seq) -> SchedAction* {
         return nullptr;
      }
   protected:
      alignas(T_ALIGN) char cache[T_SIZE];
   };
};

template<typename T_ACTION, typename ... T_ACTIONS>
struct SEQUENTIAL__ {
   using Actions = typename GenericSequential<0, 0, 0, T_ACTION, T_ACTIONS...>::Inner;
   struct Inner : SchedSequential, private Actions {
   private:
      unsigned int index = 0;
      OVERRIDE(getNext() -> SchedAction*) {
         return Actions::get(index++);
      }
   };
};


#define __sequential(...) TSL_NS::SEQUENTIAL__<__VA_ARGS__>::Inner

TSL_NS_END

#endif //TRANS_DSL_2_SEQUENTIAL_H
