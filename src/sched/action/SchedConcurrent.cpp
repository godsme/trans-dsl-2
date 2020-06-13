//
// Created by Darwin Yuan on 2020/6/13.
//

#include <trans-dsl/sched/action/SchedConcurrent.h>

TSL_NS_BEGIN

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::startUp(TransactionContext& context) -> Status {
   const auto total = getNumOfActions();
   auto workingNumber = 0;
   for(SeqInt i=0; i < total; i++) {
      auto action = get(i);
      if(action == nullptr) {
         return Result::FATAL_BUG;
      }

      ActionStatus status = action->exec(context);
      if(status.isWorking()) {
         states[i] = State::Working;
         workingNumber++;
      } else if(status.isDone()) {
         states[i] = State::Done;
      } else if(status.isFailed()) {
         states[i] = State::Done;
         return status;
      } else {
         return Result::FATAL_BUG;
      }
   }

   return workingNumber > 0 ? Result::CONTINUE : Result::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::cleanUp(TransactionContext &context) -> Status {
   const auto total = getNumOfActions();
   auto stoppingNumbers = 0;
   for(SeqInt i=0; i < total; i++) {
      if(states[i] == State::Working) {
         ActionStatus status = get(i)->stop(context);
         if(status.isFailed()) {
            runtimeContext.reportFailure(status);
            states[i] = State::Done;
         } else if(status.isWorking()) {
            stoppingNumbers++;
            states[i] = State::Working;
         } else {
            states[i] = State::Done;
         }
      }
   }

   return stoppingNumbers > 0 ? Result::CONTINUE : runtimeContext.getStatus();
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::exec(TransactionContext& context) -> Status {
   if(state != State::Idle) {
      return FATAL_BUG;
   }

   ActionStatus status = startUp(context);
   if(status.isWorking()) {
      state = State::Working;
      return status;
   } else if(status.isDone()){
      state = State::Done;
      return status;
   } else {
      Status result = cleanUp(context);
      return result == Result::SUCCESS ? (Status)status : result;
   }
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::handleEvent(TransactionContext& context, const Event& event) -> Status {
   return Result::FATAL_BUG;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::stop(TransactionContext& context) -> Status {
   return Result::FATAL_BUG;
}

///////////////////////////////////////////////////////////////////////////////
auto SchedConcurrent::kill(TransactionContext& context) -> void {
}

TSL_NS_END
