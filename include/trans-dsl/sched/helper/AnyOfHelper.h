//
// Created by Darwin Yuan on 2021/4/16.
//

#ifndef TRANS_DSL_2_ANYOFHELPER_H
#define TRANS_DSL_2_ANYOFHELPER_H

#include <trans-dsl/sched/helper/MultiAction.h>
#include <trans-dsl/sched/action/SchedAnyOf.h>
#include <trans-dsl/sched/domain/TransListenerObservedAids.h>

TSL_NS_BEGIN

namespace details {
    template<typename ... T_ACTIONS>
    struct AnyOf final  : MultiAction<SchedAnyOf, T_ACTIONS...>{
        static constexpr size_t Num_Of_Actions = sizeof...(T_ACTIONS);
        static_assert(Num_Of_Actions >= 2, "# of __any_of actions should be at least 2");
    };

    template<typename ... Ts>
    using AnyOf_t = typename AnyOf<Ts...>::template ActionRealType<EmptyAids>;
}

#define __any_of(...) TSL_NS::details::AnyOf<__VA_ARGS__>
#define __def_any_of(...) TSL_NS::details::AnyOf_t<__VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_ANYOFHELPER_H
