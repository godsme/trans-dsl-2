//
// Created by Darwin Yuan on 2020/7/2.
//

#ifndef TRANS_DSL_2_SCHEDMULTITHREAD_H
#define TRANS_DSL_2_SCHEDMULTITHREAD_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/domain/MultiThreadContext.h>
#include <cstddef>
#include <trans-dsl/sched/domain/Event.h>


TSL_NS_BEGIN

struct TransactionContext;

// 80 byte
struct SchedMultiThread : private MultiThreadContext {

   auto handleEvent(TransactionContext&, Event const&) -> Status;
   auto stop(TransactionContext&, Status) -> Status;
   auto kill(TransactionContext&, Status) -> void;

protected:
   auto start(TransactionContext&, SchedAction&) -> Status;

private:
   auto handleEvent_(ThreadId i, TransactionContext& context, Event const& event) -> Status;
   auto handleEventWorking(TransactionContext& context, Event const& event) -> Status;
   auto kill_(TransactionContext &context, Status cause) -> void;
   auto stop_(ThreadId tid, TransactionContext& context, Status cause) -> Status;
   auto stopAll(TransactionContext &context, Status cause) -> Status;
   auto othersHandleEvent(TransactionContext& context, Event const& event) -> Status;
   auto exec_(ThreadId tid, TransactionContext& context) -> Status;
   auto broadcast(TransactionContext& context, ThreadBitMap&, Event const&) -> Status;
   auto broadcastToOthers(TransactionContext& context, ThreadBitMap&, Event const& event) -> Status;
   auto scheduleEvent(TransactionContext& context, Event const& event) -> Status;
   auto notifyDoneThreads(TransactionContext& context) -> Status;
   auto stopOthers(TransactionContext& context, Status cause) -> void;
   auto joinAll(ThreadBitMap& bitMap) -> Status;
   auto checkJoinAll(TransactionContext& context) -> Status;

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
   ThreadBitMap joinBitMaps[ThreadBitMap::max]{};
   ThreadBitMap forkedBitMap{};
   ThreadBitMap newDone{};
   State state = State::INIT;
   uint8_t alive{};
   uint8_t limits{};
   ThreadId currentTid{};

private:
   OVERRIDE(join(ThreadBitMap&) -> Status);
   OVERRIDE(startThread(TransactionContext&, ThreadId) -> Status);

private:
   ABSTRACT(createThread(ThreadId) -> SchedAction*);
   ABSTRACT(getMaxThreads() const -> uint8_t);
   ABSTRACT(getThreads() -> Threads);
};


TSL_NS_END

#endif //TRANS_DSL_2_SCHEDMULTITHREAD_H
