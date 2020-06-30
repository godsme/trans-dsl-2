//
// Created by Darwin Yuan on 2020/6/8.
//

#include <trans-dsl/sched/action/SchedSequential.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/tsl_config.h>

TSL_NS_BEGIN

///////////////////////////////////////////////////////////////////////////////
#define THE_LAST_STOPPED \
   state == State::STOPPING && \
   status == Result::SUCCESS && \
   getNumOfActions() == index + 1

///////////////////////////////////////////////////////////////////////////////
inline auto SchedSequential::getFinalStatus(Status status) -> Status {
   likely_branch
   if(likely(is_working_status(status))) return status;

   unlikely_branch
   if(unlikely(THE_LAST_STOPPED)) status = Result::FORCE_STOPPED;

   state = State::DONE;

   return status;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::forward(TransactionContext& context) -> Status {
   likely_branch
   while(likely((current = getNext(index++)) != nullptr)) {
      Status status = current->exec(context);
      likely_branch
      if(likely(status != Result::SUCCESS)) {
         return status;
      }
   }

   return Result::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::exec(TransactionContext& context) -> Status {
   unlikely_branch
   if(unlikely(state != State::INIT)) {
      return Result::FATAL_BUG;
   }

   auto status = forward(context);
   state = is_working_status(status) ?  State::WORKING : State::DONE;
   return status;
}


///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::handleEvent(TransactionContext& context, Event const& event) -> Status {
//   unlikely_branch
//   if(unlikely(state != State::WORKING && state != State::STOPPING)) {
//      return Result::FATAL_BUG;
//   }

   auto status = current->handleEvent(context, event);
   if(status == SUCCESS && state == State::WORKING) {
      status = forward(context);
   }

   unlikely_branch
   if(unlikely(!(Result::__WORKING_STATUS_BEGIN & status))) {
      unlikely_branch
      if(unlikely(THE_LAST_STOPPED)) status = Result::FORCE_STOPPED;
      state = State::DONE;
   }

//   likely_branch
//   if(likely(Result::__WORKING_STATUS_BEGIN & status)) {
//      return status;
//   }
//
//   unlikely_branch
//   if(unlikely(THE_LAST_STOPPED)) status = Result::FORCE_STOPPED;
//   state = State::DONE;

   return status;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::stop(TransactionContext& context, Status cause) -> Status {
   switch (state) {
   likely_branch
   case State::WORKING: break;
   unlikely_branch
   case State::STOPPING: return Result::CONTINUE;
   unlikely_branch
   default: return Result::FATAL_BUG;
   }

   state = State::STOPPING;

   Status status = current->stop(context, cause);
   if(unlikely(status == Result::CONTINUE)) {
      return status;
   }

   return getFinalStatus(status);
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::kill(TransactionContext& context, Status cause) -> void {
   switch (state) {
   likely_branch
   case State::WORKING: [[fallthrough]];
   case State::STOPPING:
      current->kill(context, cause);
      state = State::DONE;
      return;
   unlikely_branch
   default:
      return;
   }
}


TSL_NS_END