//
// Created by Darwin Yuan on 2020/7/2.
//

#ifndef TRANS_DSL_2_MULTITHREADSCHEDULER_H
#define TRANS_DSL_2_MULTITHREADSCHEDULER_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/domain/MultiThreadContext.h>
#include <cstddef>
#include <trans-dsl/sched/domain/Event.h>
#include <trans-dsl/utils/ThreadActionTrait.h>

TSL_NS_BEGIN

struct TransactionContext;

// 80 byte
struct MultiThreadScheduler : private MultiThreadContext {

   auto handleEvent(TransactionContext&, Event const&) -> Status;
   auto stop(TransactionContext&, Status) -> Status;
   auto kill(TransactionContext&, Status) -> void;

protected:
   auto start(TransactionContext&, SchedAction&) -> Status;

private:
   auto handleEvent_(ThreadId i, TransactionContext& context, Event const& event) -> Status;
   auto handleEventWorking(TransactionContext& context, Event const& event) -> Status;
   auto kill_(TransactionContext&, Status) -> Status;
   auto stop_(TransactionContext&, Status) -> Status;
   auto kill__(TransactionContext& context, Status cause) -> void;
   auto othersHandleEvent(TransactionContext& context, Event const& event) -> Status;
   auto exec(ThreadId tid, TransactionContext& context) -> Status;
   auto broadcast(TransactionContext& context, Event const&) -> Status;
   auto broadcastToOthers(TransactionContext& context, Event const& event) -> Status;
   auto scheduleEvent(TransactionContext& context, Event const& event) -> Status;
   auto fetchADoneTid() const -> ThreadId;

protected:
   using Threads = SchedAction**;

private:
   enum class State {
      INIT,
      WORKING,
      STOPPING,
      DONE
   };
   Threads threads;
   State state = State::INIT;
   uint8_t alive{};
   uint8_t limits{};
   ThreadId currentTid{};
   ThreadBitMap newDone{};

private:
   OVERRIDE(join(ThreadBitMap) -> ThreadBitMap);
   OVERRIDE(startThread(TransactionContext&, ThreadId) -> Status);

private:
   ABSTRACT(createThread(ThreadId) -> SchedAction*);
   ABSTRACT(getMaxThreads() const -> uint8_t);
   ABSTRACT(getThreads() -> Threads);
};

template<typename MAIN_ACTION>
struct GenericMultiThreadScheduler : MultiThreadScheduler {
   auto start(TransactionContext& context) -> Status {
      return MultiThreadScheduler::start(context, mainThreadAction);
   }

private:
   OVERRIDE(createThread(ThreadId tid) -> SchedAction*) {
      return threadCreator.createThreadAction(tid);
   }

   OVERRIDE(getMaxThreads() const -> uint8_t) {
      return MAX_NUM_OF_THREADS;
   }

   OVERRIDE(getThreads() -> Threads) {
      return threads;
   }

   enum : uint8_t { MAX_NUM_OF_THREADS = details::FinalThreadCreator<MAIN_ACTION>::threadId + 1 };
   SchedAction* threads[MAX_NUM_OF_THREADS]{};
   MAIN_ACTION mainThreadAction;
   details::FinalThreadCreator<MAIN_ACTION> threadCreator;
};

TSL_NS_END

#endif //TRANS_DSL_2_MULTITHREADSCHEDULER_H
