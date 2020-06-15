//
// Created by Darwin Yuan on 2020/6/15.
//

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/porting/timer/PlatformSpecifiedTimerInfo.h>

TSL_NS_BEGIN

auto PlatformSpecifiedTimerInfo::getTimeLength(TimerId) const -> TimeLength {
   return 100;
}

TSL_NS_END