//
// Created by Darwin Yuan on 2020/7/2.
//

#ifndef TRANS_DSL_2_ASSERTIONHELPER_H
#define TRANS_DSL_2_ASSERTIONHELPER_H

#include <trans-dsl/tsl_config.h>

#define EXPECT_STATUS(call, result) do { \
   if(auto r = call; r != result) \
      return r; \
} while(0)

#define BUG_CHECK(expr) do {    \
   unlikely_branch              \
   if(unlikely(!(expr)))        \
      return Result::FATAL_BUG; \
} while(0)

#endif //TRANS_DSL_2_ASSERTIONHELPER_H
