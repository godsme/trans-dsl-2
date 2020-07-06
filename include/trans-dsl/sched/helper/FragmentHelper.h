//
// Created by Darwin Yuan on 2020/6/16.
//

#ifndef TRANS_DSL_2_FRAGMENTHELPER_H
#define TRANS_DSL_2_FRAGMENTHELPER_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/helper/AutoActionHelper.h>

#define __timer_id(V_TIMERID) TSL_NS::TimerId V_TIMERID
#define __action(T_ACTION) typename T_ACTION
#define __params(...) template < __VA_ARGS__ >
#define __def(fragment, ...) __VA_ARGS__ using fragment
#define __as(...) = TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>

#define __with(...) < __VA_ARGS__ >
#define __apply(fragment, ...) fragment __VA_ARGS__

#endif //TRANS_DSL_2_FRAGMENTHELPER_H
