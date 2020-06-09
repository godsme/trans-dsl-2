//
// Created by Darwin Yuan on 2020/6/6.
//

#ifndef TRANS_DSL_2_STATUS_H
#define TRANS_DSL_2_STATUS_H

#include <cub/cub_ns.h>
#include <cstdint>

CUB_NS_BEGIN

using Status = uint32_t;

enum : uint32_t {
   CUB_RESERVED_FAILURE = 0x80000000
};

constexpr bool is_success_status(Status status)
{
   return (status & CUB_RESERVED_FAILURE) == 0;
}

constexpr bool is_failed_status(Status status)
{
   return !is_success_status(status);
}

constexpr Status success_status(uint32_t status)
{
   return status;
}

constexpr Status fail_status(uint32_t status)
{
   return status | CUB_RESERVED_FAILURE;
}

enum Result: cub::Status
{
   SUCCESS   = cub::success_status(0),

   FATAL_BUG = cub::fail_status(0x7FFFFFFE),
   FAILURE   = cub::fail_status(0x7FFFFFFF)
};

CUB_NS_END

#endif //TRANS_DSL_2_STATUS_H
