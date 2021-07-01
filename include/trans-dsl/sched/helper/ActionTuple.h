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

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<7, ACTIONS, ELEM_TYPE> : private ACTIONS {
    inline auto get(std::size_t n) -> ELEM_TYPE* {
        switch (n) {
            case 0: return &ACTIONS::template get<0>();
            case 1: return &ACTIONS::template get<1>();
            case 2: return &ACTIONS::template get<2>();
            case 3: return &ACTIONS::template get<3>();
            case 4: return &ACTIONS::template get<4>();
            case 5: return &ACTIONS::template get<5>();
            case 6: return &ACTIONS::template get<6>();
            default: return nullptr;
        }
    }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<8, ACTIONS, ELEM_TYPE> : private ACTIONS {
    inline auto get(std::size_t n) -> ELEM_TYPE* {
        switch (n) {
            case 0: return &ACTIONS::template get<0>();
            case 1: return &ACTIONS::template get<1>();
            case 2: return &ACTIONS::template get<2>();
            case 3: return &ACTIONS::template get<3>();
            case 4: return &ACTIONS::template get<4>();
            case 5: return &ACTIONS::template get<5>();
            case 6: return &ACTIONS::template get<6>();
            case 7: return &ACTIONS::template get<7>();
            default: return nullptr;
        }
    }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<9, ACTIONS, ELEM_TYPE> : private ACTIONS {
    inline auto get(std::size_t n) -> ELEM_TYPE* {
        switch (n) {
            case 0: return &ACTIONS::template get<0>();
            case 1: return &ACTIONS::template get<1>();
            case 2: return &ACTIONS::template get<2>();
            case 3: return &ACTIONS::template get<3>();
            case 4: return &ACTIONS::template get<4>();
            case 5: return &ACTIONS::template get<5>();
            case 6: return &ACTIONS::template get<6>();
            case 7: return &ACTIONS::template get<7>();
            case 8: return &ACTIONS::template get<8>();
            default: return nullptr;
        }
    }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<10, ACTIONS, ELEM_TYPE> : private ACTIONS {
    inline auto get(std::size_t n) -> ELEM_TYPE* {
        switch (n) {
            case 0: return &ACTIONS::template get<0>();
            case 1: return &ACTIONS::template get<1>();
            case 2: return &ACTIONS::template get<2>();
            case 3: return &ACTIONS::template get<3>();
            case 4: return &ACTIONS::template get<4>();
            case 5: return &ACTIONS::template get<5>();
            case 6: return &ACTIONS::template get<6>();
            case 7: return &ACTIONS::template get<7>();
            case 8: return &ACTIONS::template get<8>();
            case 9: return &ACTIONS::template get<9>();
            default: return nullptr;
        }
    }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<11, ACTIONS, ELEM_TYPE> : private ACTIONS {
    inline auto get(std::size_t n) -> ELEM_TYPE* {
        switch (n) {
            case 0: return &ACTIONS::template get<0>();
            case 1: return &ACTIONS::template get<1>();
            case 2: return &ACTIONS::template get<2>();
            case 3: return &ACTIONS::template get<3>();
            case 4: return &ACTIONS::template get<4>();
            case 5: return &ACTIONS::template get<5>();
            case 6: return &ACTIONS::template get<6>();
            case 7: return &ACTIONS::template get<7>();
            case 8: return &ACTIONS::template get<8>();
            case 9: return &ACTIONS::template get<9>();
            case 10: return &ACTIONS::template get<10>();
            default: return nullptr;
        }
    }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<12, ACTIONS, ELEM_TYPE> : private ACTIONS {
    inline auto get(std::size_t n) -> ELEM_TYPE* {
        switch (n) {
            case 0: return &ACTIONS::template get<0>();
            case 1: return &ACTIONS::template get<1>();
            case 2: return &ACTIONS::template get<2>();
            case 3: return &ACTIONS::template get<3>();
            case 4: return &ACTIONS::template get<4>();
            case 5: return &ACTIONS::template get<5>();
            case 6: return &ACTIONS::template get<6>();
            case 7: return &ACTIONS::template get<7>();
            case 8: return &ACTIONS::template get<8>();
            case 9: return &ACTIONS::template get<9>();
            case 10: return &ACTIONS::template get<10>();
            case 11: return &ACTIONS::template get<11>();
            default: return nullptr;
        }
    }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<13, ACTIONS, ELEM_TYPE> : private ACTIONS {
    inline auto get(std::size_t n) -> ELEM_TYPE* {
        switch (n) {
            case 0: return &ACTIONS::template get<0>();
            case 1: return &ACTIONS::template get<1>();
            case 2: return &ACTIONS::template get<2>();
            case 3: return &ACTIONS::template get<3>();
            case 4: return &ACTIONS::template get<4>();
            case 5: return &ACTIONS::template get<5>();
            case 6: return &ACTIONS::template get<6>();
            case 7: return &ACTIONS::template get<7>();
            case 8: return &ACTIONS::template get<8>();
            case 9: return &ACTIONS::template get<9>();
            case 10: return &ACTIONS::template get<10>();
            case 11: return &ACTIONS::template get<11>();
            case 12: return &ACTIONS::template get<12>();
            default: return nullptr;
        }
    }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<14, ACTIONS, ELEM_TYPE> : private ACTIONS {
    inline auto get(std::size_t n) -> ELEM_TYPE* {
        switch (n) {
            case 0: return &ACTIONS::template get<0>();
            case 1: return &ACTIONS::template get<1>();
            case 2: return &ACTIONS::template get<2>();
            case 3: return &ACTIONS::template get<3>();
            case 4: return &ACTIONS::template get<4>();
            case 5: return &ACTIONS::template get<5>();
            case 6: return &ACTIONS::template get<6>();
            case 7: return &ACTIONS::template get<7>();
            case 8: return &ACTIONS::template get<8>();
            case 9: return &ACTIONS::template get<9>();
            case 10: return &ACTIONS::template get<10>();
            case 11: return &ACTIONS::template get<11>();
            case 12: return &ACTIONS::template get<12>();
            case 13: return &ACTIONS::template get<13>();
            default: return nullptr;
        }
    }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<15, ACTIONS, ELEM_TYPE> : private ACTIONS {
    inline auto get(std::size_t n) -> ELEM_TYPE* {
        switch (n) {
            case 0: return &ACTIONS::template get<0>();
            case 1: return &ACTIONS::template get<1>();
            case 2: return &ACTIONS::template get<2>();
            case 3: return &ACTIONS::template get<3>();
            case 4: return &ACTIONS::template get<4>();
            case 5: return &ACTIONS::template get<5>();
            case 6: return &ACTIONS::template get<6>();
            case 7: return &ACTIONS::template get<7>();
            case 8: return &ACTIONS::template get<8>();
            case 9: return &ACTIONS::template get<9>();
            case 10: return &ACTIONS::template get<10>();
            case 11: return &ACTIONS::template get<11>();
            case 12: return &ACTIONS::template get<12>();
            case 13: return &ACTIONS::template get<13>();
            case 14: return &ACTIONS::template get<14>();
            default: return nullptr;
        }
    }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<16, ACTIONS, ELEM_TYPE> : private ACTIONS {
    inline auto get(std::size_t n) -> ELEM_TYPE* {
        switch (n) {
            case 0: return &ACTIONS::template get<0>();
            case 1: return &ACTIONS::template get<1>();
            case 2: return &ACTIONS::template get<2>();
            case 3: return &ACTIONS::template get<3>();
            case 4: return &ACTIONS::template get<4>();
            case 5: return &ACTIONS::template get<5>();
            case 6: return &ACTIONS::template get<6>();
            case 7: return &ACTIONS::template get<7>();
            case 8: return &ACTIONS::template get<8>();
            case 9: return &ACTIONS::template get<9>();
            case 10: return &ACTIONS::template get<10>();
            case 11: return &ACTIONS::template get<11>();
            case 12: return &ACTIONS::template get<12>();
            case 13: return &ACTIONS::template get<13>();
            case 14: return &ACTIONS::template get<14>();
            case 15: return &ACTIONS::template get<15>();
            default: return nullptr;
        }
    }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<17, ACTIONS, ELEM_TYPE> : private ACTIONS {
    inline auto get(std::size_t n) -> ELEM_TYPE* {
        switch (n) {
            case 0: return &ACTIONS::template get<0>();
            case 1: return &ACTIONS::template get<1>();
            case 2: return &ACTIONS::template get<2>();
            case 3: return &ACTIONS::template get<3>();
            case 4: return &ACTIONS::template get<4>();
            case 5: return &ACTIONS::template get<5>();
            case 6: return &ACTIONS::template get<6>();
            case 7: return &ACTIONS::template get<7>();
            case 8: return &ACTIONS::template get<8>();
            case 9: return &ACTIONS::template get<9>();
            case 10: return &ACTIONS::template get<10>();
            case 11: return &ACTIONS::template get<11>();
            case 12: return &ACTIONS::template get<12>();
            case 13: return &ACTIONS::template get<13>();
            case 14: return &ACTIONS::template get<14>();
            case 15: return &ACTIONS::template get<15>();
            case 16: return &ACTIONS::template get<16>();
            default: return nullptr;
        }
    }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<18, ACTIONS, ELEM_TYPE> : private ACTIONS {
    inline auto get(std::size_t n) -> ELEM_TYPE* {
        switch (n) {
            case 0: return &ACTIONS::template get<0>();
            case 1: return &ACTIONS::template get<1>();
            case 2: return &ACTIONS::template get<2>();
            case 3: return &ACTIONS::template get<3>();
            case 4: return &ACTIONS::template get<4>();
            case 5: return &ACTIONS::template get<5>();
            case 6: return &ACTIONS::template get<6>();
            case 7: return &ACTIONS::template get<7>();
            case 8: return &ACTIONS::template get<8>();
            case 9: return &ACTIONS::template get<9>();
            case 10: return &ACTIONS::template get<10>();
            case 11: return &ACTIONS::template get<11>();
            case 12: return &ACTIONS::template get<12>();
            case 13: return &ACTIONS::template get<13>();
            case 14: return &ACTIONS::template get<14>();
            case 15: return &ACTIONS::template get<15>();
            case 16: return &ACTIONS::template get<16>();
            case 17: return &ACTIONS::template get<17>();
            default: return nullptr;
        }
    }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<19, ACTIONS, ELEM_TYPE> : private ACTIONS {
    inline auto get(std::size_t n) -> ELEM_TYPE* {
        switch (n) {
            case 0: return &ACTIONS::template get<0>();
            case 1: return &ACTIONS::template get<1>();
            case 2: return &ACTIONS::template get<2>();
            case 3: return &ACTIONS::template get<3>();
            case 4: return &ACTIONS::template get<4>();
            case 5: return &ACTIONS::template get<5>();
            case 6: return &ACTIONS::template get<6>();
            case 7: return &ACTIONS::template get<7>();
            case 8: return &ACTIONS::template get<8>();
            case 9: return &ACTIONS::template get<9>();
            case 10: return &ACTIONS::template get<10>();
            case 11: return &ACTIONS::template get<11>();
            case 12: return &ACTIONS::template get<12>();
            case 13: return &ACTIONS::template get<13>();
            case 14: return &ACTIONS::template get<14>();
            case 15: return &ACTIONS::template get<15>();
            case 16: return &ACTIONS::template get<16>();
            case 17: return &ACTIONS::template get<17>();
            case 18: return &ACTIONS::template get<18>();
            default: return nullptr;
        }
    }
};

template<typename ACTIONS, typename ELEM_TYPE>
struct Actions<20, ACTIONS, ELEM_TYPE> : private ACTIONS {
    inline auto get(std::size_t n) -> ELEM_TYPE* {
        switch (n) {
            case 0: return &ACTIONS::template get<0>();
            case 1: return &ACTIONS::template get<1>();
            case 2: return &ACTIONS::template get<2>();
            case 3: return &ACTIONS::template get<3>();
            case 4: return &ACTIONS::template get<4>();
            case 5: return &ACTIONS::template get<5>();
            case 6: return &ACTIONS::template get<6>();
            case 7: return &ACTIONS::template get<7>();
            case 8: return &ACTIONS::template get<8>();
            case 9: return &ACTIONS::template get<9>();
            case 10: return &ACTIONS::template get<10>();
            case 11: return &ACTIONS::template get<11>();
            case 12: return &ACTIONS::template get<12>();
            case 13: return &ACTIONS::template get<13>();
            case 14: return &ACTIONS::template get<14>();
            case 15: return &ACTIONS::template get<15>();
            case 16: return &ACTIONS::template get<16>();
            case 17: return &ACTIONS::template get<17>();
            case 18: return &ACTIONS::template get<18>();
            case 19: return &ACTIONS::template get<19>();
            default: return nullptr;
        }
    }
};

template<typename ... ACTIONS>
using ActionTuple = Actions<sizeof...(ACTIONS), tuple<ACTIONS...>, SchedAction>;

TSL_NS_END

#endif //TRANS_DSL_2_ACTIONTUPLE_H
