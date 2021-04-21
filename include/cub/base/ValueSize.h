//
// Created by Darwin Yuan on 2021/4/21.
//

#ifndef TRANS_DSL_2_VALUESIZE_H
#define TRANS_DSL_2_VALUESIZE_H

#include <cub/cub_ns.h>
#include <cstdint>
#include <numeric>

CUB_NS_BEGIN

namespace detail {
    template<std::size_t MAX_SIZE>
    auto DeduceValueSize() -> auto {
        if constexpr (MAX_SIZE <= std::numeric_limits<uint8_t>::max()) {
            return uint8_t{};
        } else if constexpr (MAX_SIZE <= std::numeric_limits<uint16_t>::max()) {
            return uint16_t{};
        } else if(sizeof(std::size_t) == sizeof(uint32_t) || MAX_SIZE <= std::numeric_limits<uint32_t>::max()) {
            return uint32_t{};
        } else {
            return uint64_t{};
        }
    }
}

template<std::size_t MAX_SIZE>
using ValueSize_t = decltype(detail::DeduceValueSize<MAX_SIZE>());

CUB_NS_END

#endif //TRANS_DSL_2_VALUESIZE_H
