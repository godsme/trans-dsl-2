//
// Created by godsme on 6/19/20.
//

#ifndef TRANS_DSL_2_SCHEDACTIONCONCEPT_H
#define TRANS_DSL_2_SCHEDACTIONCONCEPT_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/sched/domain/SchedAction.h>
#include <type_traits>

#if __has_include(<concepts>)
#include <concepts>
#endif

TSL_NS_BEGIN

template<typename T>
constexpr bool IsSchedAction = std::is_base_of_v<SchedAction, T>;

#if __CONCEPT_ENABLED
template<typename T>
concept SchedActionConcept = IsSchedAction<T>;
#else
template<typename T>
using SchedActionConcept = std::enable_if_t<IsSchedAction<T>>;
#endif

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDACTIONCONCEPT_H
