//
// Created by Darwin Yuan on 2021/3/22.
//

#ifndef TRANS_DSL_2_EXCLUSIVEHELPER_H
#define TRANS_DSL_2_EXCLUSIVEHELPER_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/sched/helper/ActionTuple.h>
#include <trans-dsl/sched/helper/MultiAction.h>
#include <trans-dsl/sched/action/SchedExclusive.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/domain/TransListenerObservedAids.h>

TSL_NS_BEGIN

struct SchedAction;

namespace details {

    template<typename ... T_ACTIONS>
    struct Exclusive final  : MultiAction<SchedExclusive, T_ACTIONS...>{
        static constexpr size_t Num_Of_Actions = sizeof...(T_ACTIONS);
        static_assert(Num_Of_Actions >= 2, "# of exclusive actions should be at least 2");
    };

    template<typename ... Ts>
    using Exclusive_t = typename Exclusive<Ts...>::template ActionRealType<EmptyAids>;
}

#define __exclusive(...) TSL_NS::details::Exclusive<__VA_ARGS__>
#define __def_exclusive(...) TSL_NS::details::Exclusive_t<__VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_EXCLUSIVEHELPER_H
