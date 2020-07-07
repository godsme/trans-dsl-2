//
// Created by Darwin Yuan on 2020/7/7.
//

#ifndef TRANS_DSL_2_OBSERVEDACTIONIDREGISTRY_H
#define TRANS_DSL_2_OBSERVEDACTIONIDREGISTRY_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/domain/ActionId.h>
#include <cub/base/BitSet.h>

TSL_NS_BEGIN

template<ActionId ... AIDs>
struct ObservedActionIdRegistry {
private:
   using BitSet = cub::BitSet<uint64_t>;
   constexpr static BitSet _Aids = ( ((uint64_t)1 << AIDs) | ... | 0 );
public:
   constexpr static BitSet Aids = _Aids.empty() ? 1 : _Aids;
};

TSL_NS_END

#endif //TRANS_DSL_2_OBSERVEDACTIONIDREGISTRY_H
