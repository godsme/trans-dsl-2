//
// Created by Darwin Yuan on 2020/6/27.
//

#ifndef TRANS_DSL_2_MAXSIZECALC_H
#define TRANS_DSL_2_MAXSIZECALC_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/SeqInt.h>
#include <cstddef>
#include <algorithm>

TSL_NS_BEGIN

namespace details {
   struct MaxSizeCalc {
      constexpr MaxSizeCalc(size_t size = 0) : size(size) {}
      constexpr operator size_t() const { return size; }
      size_t size = 0;
   };

   constexpr MaxSizeCalc operator<<(MaxSizeCalc calc, size_t size) {
      return std::max(calc.size, size);
   }
}

TSL_NS_END

#endif //TRANS_DSL_2_MAXSIZECALC_H
