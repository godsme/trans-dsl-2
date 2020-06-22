//
// Created by Darwin Yuan on 2020/6/22.
//

#ifndef TRANS_DSL_2_CLASSCONCEPT_H
#define TRANS_DSL_2_CLASSCONCEPT_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/concepts/ConceptHelper.h>

#if __has_include(<concepts>)
#include <concepts>
#endif

TSL_NS_BEGIN

template<typename T>
DEF_CONCEPT(ClassConcept, std::is_class_v<T>);

TSL_NS_END

#endif //TRANS_DSL_2_CLASSCONCEPT_H
