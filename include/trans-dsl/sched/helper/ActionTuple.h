//
// Created by Darwin Yuan on 2020/8/31.
//

#ifndef TRANS_DSL_2_ACTIONTUPLE_H
#define TRANS_DSL_2_ACTIONTUPLE_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/tuple_t.h>
#include <trans-dsl/sched/domain/SchedAction.h>

TSL_NS_BEGIN

template<std::size_t N, typename ACTIONS, typename ELEM_TYPE>
struct Actions;

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<2, ACTIONS, ELEM_TYPE> : private ACTIONS {
   inline auto get(std::size_t n) -> ELEM_TYPE* {
      switch (n) {
         case 0: return &ACTIONS::template get<0>();
         case 1: return &ACTIONS::template get<1>();
         default: return nullptr;
      }
   }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<3, ACTIONS, ELEM_TYPE> : private ACTIONS {
   inline auto get(std::size_t n) -> ELEM_TYPE* {
      switch (n) {
         case 0: return &ACTIONS::template get<0>();
         case 1: return &ACTIONS::template get<1>();
         case 2: return &ACTIONS::template get<2>();
         default: return nullptr;
      }
   }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<4, ACTIONS, ELEM_TYPE> : private ACTIONS {
   inline auto get(std::size_t n) -> ELEM_TYPE* {
      switch (n) {
         case 0: return &ACTIONS::template get<0>();
         case 1: return &ACTIONS::template get<1>();
         case 2: return &ACTIONS::template get<2>();
         case 3: return &ACTIONS::template get<3>();
         default: return nullptr;
      }
   }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<5, ACTIONS, ELEM_TYPE> : private ACTIONS {
   inline auto get(std::size_t n) -> ELEM_TYPE* {
      switch (n) {
         case 0: return &ACTIONS::template get<0>();
         case 1: return &ACTIONS::template get<1>();
         case 2: return &ACTIONS::template get<2>();
         case 3: return &ACTIONS::template get<3>();
         case 4: return &ACTIONS::template get<4>();
         default: return nullptr;
      }
   }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<6, ACTIONS, ELEM_TYPE> : private ACTIONS {
   inline auto get(std::size_t n) -> ELEM_TYPE* {
      switch (n) {
         case 0: return &ACTIONS::template get<0>();
         case 1: return &ACTIONS::template get<1>();
         case 2: return &ACTIONS::template get<2>();
         case 3: return &ACTIONS::template get<3>();
         case 4: return &ACTIONS::template get<4>();
         case 5: return &ACTIONS::template get<5>();
         default: return nullptr;
      }
   }
};

template<typename ... ACTIONS>
using ActionTuple = Actions<sizeof...(ACTIONS), tuple<ACTIONS...>, SchedAction>;

TSL_NS_END

#endif //TRANS_DSL_2_ACTIONTUPLE_H
