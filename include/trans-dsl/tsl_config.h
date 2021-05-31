//
// Created by godsme on 6/19/20.
//

#ifndef TRANS_DSL_2_TSL_CONFIG_H
#define TRANS_DSL_2_TSL_CONFIG_H

#if __has_include(<concepts>)
#define __CONCEPT_ENABLED 0
#else
#define __CONCEPT_ENABLED 0
#endif

#if __cplusplus < 201709L
#define consteval constexpr
#endif

#if __cplusplus >= 201709L
#define likely_branch [[likely]]
#define unlikely_branch [[unlikely]]
#define likely(x) x
#define unlikely(x) x
#else
#define likely_branch
#define unlikely_branch

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#endif //TRANS_DSL_2_TSL_CONFIG_H
