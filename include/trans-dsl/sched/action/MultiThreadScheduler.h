//
// Created by Darwin Yuan on 2020/7/2.
//

#ifndef TRANS_DSL_2_MULTITHREADSCHEDULER_H
#define TRANS_DSL_2_MULTITHREADSCHEDULER_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/domain/MultiThreadContext.h>
#include <cstddef>
#include <trans-dsl/sched/domain/Event.h>

TSL_NS_BEGIN

struct TransactionContext;

struct MultiThreadScheduler
   : private MultiThreadContext {

   auto start(TransactionContext&, SchedAction&) -> Status;
   auto handleEvent(TransactionContext&, Event const&) -> Status;
   auto stop(TransactionContext&, Status) -> Status;
   auto kill(TransactionContext&, Status) -> void;

private:
   auto handleEvent(ThreadId i, TransactionContext& context, Event const& event) -> Status;
   auto handleEventWorking(TransactionContext& context, Event const& event) -> Status;
   auto kill_(TransactionContext&, Status) -> Status;
   auto stop_(TransactionContext&, Status) -> Status;
   auto kill__(TransactionContext& context, Status cause) -> void;
   auto slavesHandleEvent(TransactionContext& context, Event const& event) -> Status;

private:
   static constexpr size_t MAX_NUM_OF_THREADS = sizeof(ThreadBitMap) * 8;
   SchedAction* actions[MAX_NUM_OF_THREADS]{};
   enum class State {
      INIT,
      WORKING,
      STOPPING,
      DONE
   };

   State state = State::INIT;
   uint8_t alive{};
   uint8_t limits{};

private:
   OVERRIDE(join(ThreadBitMap) -> ThreadBitMap);
   OVERRIDE(startThread(ThreadId, SchedAction&) -> Status);
};

TSL_NS_END

#endif //TRANS_DSL_2_MULTITHREADSCHEDULER_H
