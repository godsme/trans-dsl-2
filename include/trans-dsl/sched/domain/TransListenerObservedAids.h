//
// Created by Darwin Yuan on 2020/7/7.
//

#ifndef TRANS_DSL_2_TRANSLISTENEROBSERVEDAIDS_H
#define TRANS_DSL_2_TRANSLISTENEROBSERVEDAIDS_H

#include <trans-dsl/tsl_ns.h>
#include <cub/base/BitS.h>
#include <cstdint>

TSL_NS_BEGIN

using TransListenerObservedAids = cub::BitS<uint64_t>;

inline constexpr TransListenerObservedAids EmptyAids{0};

TSL_NS_END

#endif //TRANS_DSL_2_TRANSLISTENEROBSERVEDAIDS_H
