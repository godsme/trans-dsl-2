//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_TSL_STATUS_H
#define TRANS_DSL_2_TSL_STATUS_H

#include <trans-dsl/tsl_ns.h>
#include <cub/base/Status.h>

TSL_NS_BEGIN

enum class Status : CUB_NS::Status {
   SUCCESS   = CUB_NS::Status(CUB_NS::CubStatus::SUCCESS),

   FATAL_BUG = CUB_NS::Status(CUB_NS::CubStatus::FATAL_BUG),
   FAILED    = CUB_NS::Status(CUB_NS::CubStatus::FAILURE),

   RESERVED_FAILURE = CUB_NS::CUB_RESERVED_FAILURE,

   __TSL_STATUS_BEGIN = 100,

   CONTINUE       = CUB_NS::success_status(__TSL_STATUS_BEGIN),
   UNKNOWN_EVENT,

   OUT_OF_SCOPE   = CUB_NS::fail_status(__TSL_STATUS_BEGIN),
   DUPTID,
   INVALID_DATA,
   TIMEDOUT,
   RESTART_REQUIRED,
   NOTHING_CHANGED,

   FORCE_STOPPED
};

TSL_NS_END

#endif //TRANS_DSL_2_TSL_STATUS_H
