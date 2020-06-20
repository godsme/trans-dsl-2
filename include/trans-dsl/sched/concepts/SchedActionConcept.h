//
// Created by godsme on 6/19/20.
//

#ifndef TRANS_DSL_2_SCHEDACTIONCONCEPT_H
#define TRANS_DSL_2_SCHEDACTIONCONCEPT_H

#if __has_include(<concepts>)

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/domain/Event.h>
#include <trans-dsl/tsl_status.h>
#include <concepts>
#include <trans-dsl/sched/domain/SchedAction.h>
#include <type_traits>

TSL_NS_BEGIN

template<typename T>
concept SchedActionConcept = std::is_base_of_v<SchedAction, T>;

TSL_NS_END

#endif

#endif //TRANS_DSL_2_SCHEDACTIONCONCEPT_H
