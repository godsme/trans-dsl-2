//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_ISCLASS_H
#define TRANS_DSL_2_ISCLASS_H

#include <cub/cub_ns.h>
#include <type_traits>

CUB_NS_BEGIN

template<typename T>
using IsClass = std::enable_if_t<std::is_class_v<T>, void>;

CUB_NS_END

#endif //TRANS_DSL_2_ISCLASS_H
