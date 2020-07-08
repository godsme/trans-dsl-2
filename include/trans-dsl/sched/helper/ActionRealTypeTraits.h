//
// Created by Darwin Yuan on 2020/7/7.
//

#ifndef TRANS_DSL_2_ACTIONREALTYPETRAITS_H
#define TRANS_DSL_2_ACTIONREALTYPETRAITS_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/domain/TransListenerObservedAids.h>

TSL_NS_BEGIN

template<TransListenerObservedAids const& AIDs, typename T, typename = void>
struct ActionRealTypeTraits {
   using type = T;
};

template<TransListenerObservedAids const& AIDs, typename T>
struct ActionRealTypeTraits<AIDs, T, std::void_t<typename T::template ActionRealType<AIDs> >> {
   using type = typename T::template ActionRealType<AIDs>;
};

template<TransListenerObservedAids const& AIDs, typename T>
using ActionRealTypeTraits_t = typename ActionRealTypeTraits<AIDs, T, void>::type;

TSL_NS_END

#endif //TRANS_DSL_2_ACTIONREALTYPETRAITS_H
