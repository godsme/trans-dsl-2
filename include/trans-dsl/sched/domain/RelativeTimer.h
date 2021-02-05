//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_RELATIVETIMER_H
#define TRANS_DSL_2_RELATIVETIMER_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/porting/timer/TimerId.h>
#include <trans-dsl/porting/timer/TimeLength.h>
#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/domain/Event.h>
#include <cub/base/Keywords.h>
#include <cub/dci/Role.h>

TSL_NS_BEGIN

struct TransactionInfo;
struct TimerInfo;

DEFINE_ROLE(RelativeTimer)
{
   explicit RelativeTimer(TimerId timerId);

   Status start(TimerInfo const&, TransactionInfo const&);
   void stop();

   bool matches(Event const& event) const;

private:
   const TimerId timerId;
   bool started = false;

private:
   ABSTRACT(startTimer_(TimerId, TimeLength timerLen) -> Status);
   ABSTRACT(stopTimer_(TimerId) -> void);
   ABSTRACT(matches_(Event const&, TimerId) const -> bool);
};

TSL_NS_END

#endif //TRANS_DSL_2_RELATIVETIMER_H
