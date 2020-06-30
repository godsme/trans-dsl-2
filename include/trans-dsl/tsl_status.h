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
   MOVE_ON, // reserved (for loop)

   FATAL_BUG = Status(CUB_NS::Result::FATAL_BUG),
   FAILED    = Status(CUB_NS::Result::FAILURE),

   RESERVED_FAILURE = CUB_NS::CUB_RESERVED_FAILURE,
   UNSPECIFIED, // reserved (for loop)

   OUT_OF_SCOPE   = CUB_NS::fail_status(100),
   DUPTID,
   INVALID_DATA,
   TIMEOUT,
   USER_FATAL_BUG,
   RESTART_REQUIRED,
   NOTHING_CHANGED,

   FORCE_STOPPED,

   ////////////////////////////////////////////////////
   __WORKING_STATUS_BEGIN = 0x4000'0000,
   CONTINUE       = __WORKING_STATUS_BEGIN,
   UNKNOWN_EVENT  = __WORKING_STATUS_BEGIN + 1
};

inline constexpr auto is_working_status(Status status) -> bool {
   return (Result::__WORKING_STATUS_BEGIN & status) > 0;
}

inline constexpr auto is_not_working_status(Status status) -> bool {
   return !(Result::__WORKING_STATUS_BEGIN & status);
}

TSL_NS_END

#endif //TRANS_DSL_2_TSL_STATUS_H
