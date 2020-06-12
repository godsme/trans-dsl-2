//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_TSL_STATUS_H
#define TRANS_DSL_2_TSL_STATUS_H

#include <trans-dsl/tsl_ns.h>
#include <cub/base/Status.h>

TSL_NS_BEGIN

using Status = CUB_NS::Status;

enum Result : Status {
   SUCCESS   = Status(CUB_NS::Result::SUCCESS),

   FATAL_BUG = Status(CUB_NS::Result::FATAL_BUG),
   FAILED    = Status(CUB_NS::Result::FAILURE),

   RESERVED_FAILURE = CUB_NS::CUB_RESERVED_FAILURE,
   UNSPECIFIED, // reserved

   __TSL_STATUS_BEGIN = 100,

   CONTINUE       = CUB_NS::success_status(__TSL_STATUS_BEGIN),
   UNKNOWN_EVENT,
   MOVE_ON, // reserved

   OUT_OF_SCOPE   = CUB_NS::fail_status(__TSL_STATUS_BEGIN),
   DUPTID,
   INVALID_DATA,
   TIMEDOUT,
   USER_FATAL_BUG,
   RESTART_REQUIRED,
   NOTHING_CHANGED,


   FORCE_STOPPED
};

TSL_NS_END

#endif //TRANS_DSL_2_TSL_STATUS_H
