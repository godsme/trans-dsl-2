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

#if __CONCEPT_ENABLED
template<typename T>
concept SchedActionConcept = std::is_base_of_v<SchedAction, T>;
#else
template<typename T>
using SchedActionConcept = std::enable_if_t<std::is_base_of_v<TSL_NS::SchedAction, T>>;
#endif

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDACTIONCONCEPT_H
