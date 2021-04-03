//
// Created by Darwin Yuan on 2020/6/8.
//

#include <trans-dsl/sched/action/SchedSequential.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/tsl_config.h>

TSL_NS_BEGIN

///////////////////////////////////////////////////////////////////////////////
inline auto SchedSequential::getFinalStatus(Status status) -> Status {
   likely_branch
   if(likely(is_working_status(status))) return status;

   unlikely_branch
   if(unlikely(state == State::STOPPING &&
           status == Result::SUCCESS &&
           getNumOfActions() > index)) status = stopCause;

   state = State::DONE;

   return status;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::forward(TransactionContext& context) -> Status {
   likely_branch
   while(likely((current = getNext(index++)) != nullptr)) {
      auto status = current->exec(context);
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
   state = (Result::CONTINUE == status) ?  State::WORKING : State::DONE;
   return status;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::handleEvent_(TransactionContext &context, const Event &event) -> Status {
   // This is the most invoked main path, we expand the
   // the specific implementation here for best performance,
   // although there are some duplications with other paths.
   auto status = current->handleEvent(context, event);
   if (status == SUCCESS) {
      status = forward(context);
   }
   unlikely_branch
   if (unlikely(!(Result::__WORKING_STATUS_BEGIN & status))) {
      state = State::DONE;
   }
   return status;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::handleEvent(TransactionContext& context, Event const& event) -> Status {
   switch (state) {
      likely_branch
      case State::WORKING: {
         return handleEvent_(context, event);
      }
      case State::STOPPING: {
         return getFinalStatus(current->handleEvent(context, event));
      }
      unlikely_branch
      default: {
         return Result::FATAL_BUG;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
auto SchedSequential::stop(TransactionContext& context, Status cause) -> Status {
   switch (state) {
      likely_branch
      case State::WORKING: {
         state = State::STOPPING;
         stopCause = cause;
         return getFinalStatus(current->stop(context, cause));
      }
      unlikely_branch
      case State::STOPPING: {
         return Result::CONTINUE;
      }
      unlikely_branch
      default: {
         return Result::FATAL_BUG;
      }
   }
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