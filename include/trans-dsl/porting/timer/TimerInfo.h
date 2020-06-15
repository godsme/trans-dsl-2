//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_TIMERINFO_H
#define TRANS_DSL_2_TIMERINFO_H

#include <trans-dsl/tsl_ns.h>
#include <cub/dci/Role.h>
#include <trans-dsl/porting/timer/TimerId.h>
#include <trans-dsl/porting/timer/TimeLength.h>

TSL_NS_BEGIN

DEFINE_ROLE(TimerInfo) {
  ABSTRACT(getTimeLength(TimerId) const -> TimeLength);
};

TSL_NS_END

#endif //TRANS_DSL_2_TIMERINFO_H
