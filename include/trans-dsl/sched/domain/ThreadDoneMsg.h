//
// Created by Darwin Yuan on 2020/7/1.
//

#ifndef TRANS_DSL_2_THREADDONEMSG_H
#define TRANS_DSL_2_THREADDONEMSG_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/domain/EventId.h>
#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/domain/ThreadId.h>

TSL_NS_BEGIN

const EventId EV_ACTION_THREAD_DONE = 0xFFFE;

struct ThreadDoneMsg
{
   ThreadDoneMsg(ThreadId who, Status status)
      : who(who)
      , result(status)
   {}

   const ThreadId     who;
   const cub::Status  result;
};

TSL_NS_END

#endif //TRANS_DSL_2_THREADDONEMSG_H
