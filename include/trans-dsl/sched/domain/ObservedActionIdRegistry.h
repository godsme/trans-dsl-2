//
// Created by Darwin Yuan on 2020/7/7.
//

#ifndef TRANS_DSL_2_OBSERVEDACTIONIDREGISTRY_H
#define TRANS_DSL_2_OBSERVEDACTIONIDREGISTRY_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/domain/ActionId.h>
#include <cub/base/BitS.h>
#include <trans-dsl/sched/domain/TransListenerObservedAids.h>

TSL_NS_BEGIN

template<ActionId ... AIDs>
struct ObservedActionIdRegistry {
private:
   using BitSet = TransListenerObservedAids;
   static_assert(sizeof...(AIDs) < sizeof(BitSet) * 8, "too many AIDs");
   constexpr static BitSet _Aids = ( BitSet{((uint64_t)1 << AIDs)} | ... | BitSet{0} );
public:
   constexpr static BitSet Aids = _Aids.empty() ? 1 : _Aids;
};

TSL_NS_END

#endif //TRANS_DSL_2_OBSERVEDACTIONIDREGISTRY_H
