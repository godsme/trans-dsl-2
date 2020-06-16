//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_PLATFORMSPECIFIEDTIMERINFO_H
#define TRANS_DSL_2_PLATFORMSPECIFIEDTIMERINFO_H

#include <trans-dsl/sched/domain/TimerInfo.h>

TSL_NS_BEGIN

struct PlatformSpecifiedTimerInfo : TimerInfo {
   OVERRIDE(getTimeLength(TimerId) const -> TimeLength);
};

TSL_NS_END

#endif //TRANS_DSL_2_PLATFORMSPECIFIEDTIMERINFO_H
