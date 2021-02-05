//
// Created by Darwin Yuan on 2020/6/15.
//

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/porting/timer/PlatformSpecifiedTimerInfo.h>
#include <trans-dsl/action/TransactionInfo.h>

TSL_NS_BEGIN

auto PlatformSpecifiedTimerInfo::getTimeLength(TimerId, TransactionInfo const&) const -> TimeLength {
   return 100;
}

TSL_NS_END