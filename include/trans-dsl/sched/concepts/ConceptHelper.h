//
// Created by Darwin Yuan on 2020/6/20.
//

#ifndef TRANS_DSL_2_CONCEPTHELPER_H
#define TRANS_DSL_2_CONCEPTHELPER_H

#include <trans-dsl/tsl_config.h>
#include <type_traits>

#if __CONCEPT_ENABLED
#define VOID_PLACEHOLDER
#define VOID_CONCEPT

#define CONCEPT_C(c, t) c t
#define ENABLE_C(c, t)
#define DEF_CONCEPT(c, ...) concept c = __VA_ARGS__
#define CONCEPT(c) c
#else
#define VOID_PLACEHOLDER , void
#define VOID_PLACEHOLDER_2  void,
#define VOID_CONCEPT , typename = void
#define VOID_CONCEPT_2 typename = void ,
#define CONCEPT_C(c, t) typename t
#define ENABLE_C(c, ...) , std::enable_if_t<c<__VA_ARGS__>>
#define ENABLE_C_2(c, ...) std::enable_if_t<c<__VA_ARGS__>> ,
#define DEF_CONCEPT(c, ...) constexpr bool c = __VA_ARGS__
#define CONCEPT(c) typename
#endif

#if !__CONCEPT_ENABLED

namespace details {
   template<typename T>
   constexpr bool AlwaysTrue = true;
}

template<typename C, typename T>
constexpr static bool IsTypeClass = details::AlwaysTrue<decltype(C::template deduceType<T>)>;

#define __METHOD(result, ...) static_assert(std::is_same_v<result, decltype(__VA_ARGS__)>)
#define __DEF_TYPE_CLASS(...) static decltype(auto) deduceType(__VA_ARGS__)

#endif

#endif //TRANS_DSL_2_CONCEPTHELPER_H
