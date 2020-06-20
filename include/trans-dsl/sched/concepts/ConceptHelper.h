//
// Created by Darwin Yuan on 2020/6/20.
//

#ifndef TRANS_DSL_2_CONCEPTHELPER_H
#define TRANS_DSL_2_CONCEPTHELPER_H

#include <trans-dsl/tsl_config.h>

#if __CONCEPT_ENABLED
#define VOID_PLACEHOLDER
#define VOID_CONCEPT

#define CONCEPT_C(c, t) c t
#define ENABLE_C(c, t)
#define DEF_CONCEPT(c, ...) concept c = __VA_ARGS__
#else
#define VOID_PLACEHOLDER , void
#define VOID_CONCEPT , typename = void
#define CONCEPT_C(c, t) typename t
#define ENABLE_C(c, t) , c<t>
#define DEF_CONCEPT(c, ...) using c = std::enable_if_t<__VA_ARGS__>
#endif

#endif //TRANS_DSL_2_CONCEPTHELPER_H
