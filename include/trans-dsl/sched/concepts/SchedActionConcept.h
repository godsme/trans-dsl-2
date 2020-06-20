//
// Created by godsme on 6/19/20.
//

#ifndef TRANS_DSL_2_SCHEDACTIONCONCEPT_H
#define TRANS_DSL_2_SCHEDACTIONCONCEPT_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/sched/domain/SchedAction.h>
#include <type_traits>
#include <trans-dsl/sched/concepts/ConceptHelper.h>
#if __has_include(<concepts>)
#include <concepts>
#endif

TSL_NS_BEGIN

template<typename T>
DEF_CONCEPT(SchedActionConcept, std::is_base_of_v<SchedAction, T>);

TSL_NS_END

#endif //TRANS_DSL_2_SCHEDACTIONCONCEPT_H
