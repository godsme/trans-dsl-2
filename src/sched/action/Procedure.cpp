//
// Created by Darwin Yuan on 2020/6/7.
//

#include <trans-dsl/sched/action/Procedure.h>
#include <cub/dci/Role.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/concept/SchedAction.h>
#include <cub/gof/Singleton.h>

TSL_NS_BEGIN

DEFINE_ROLE(Procedure::State) {
   DEFAULT(enter(Procedure&, TransactionContext&, Status status) -> Status) {
      return status;
   }

   DEFAULT(exec(Procedure&, TransactionContext&) -> Status) {
      return Result::FATAL_BUG;
   }

   DEFAULT(handleEvent(Procedure&, TransactionContext&, const Event&) -> Status) {
      return Result::FATAL_BUG;
   }
};

template<typename T>
inline Status Procedure::gotoState(TransactionContext& context, Status status) {
   state = &T::getInstance();
   return state->enter(*this, context, status);
}

DEF_SINGLETON(Procedure::Idle, Procedure::State)  {
   OVERRIDE(enter(Procedure& this__, TransactionContext&, Status status) -> Status) {
      if(this__.action = this__.getAction(); this__.action == nullptr) {
         return Result::FATAL_BUG;
      }

      return status;
   }

   OVERRIDE(exec(Procedure& this__, TransactionContext& context) -> Status) {
      auto status = this__.action->exec(context);
      if(status == Result::CONTINUE) {
         return this__.gotoState<Working>(context, status);
      } else {
         return this__.gotoState<Stopping>(context, status);
      }
   }
};

DEF_SINGLETON(Procedure::Working, Procedure::State) {
   OVERRIDE(handleEvent(Procedure& this__, TransactionContext& context, const Event& event) -> Status) {
      ActionStatus status = this__.action->handleEvent(context, event);
      if(status.isWorking()) {
         return status;
      } else {
         return this__.gotoState<Stopping>(context, status);
      }
   }
};

DEF_SINGLETON(Procedure::Stopping, Procedure::State) {
   OVERRIDE(enter(Procedure& this__, TransactionContext& context, Status result) -> Status) {
      if(this__.action = this__.getFinalAction(); this__.action == nullptr) {
         return Result::FATAL_BUG;
      }

      ActionStatus status = this__.action->exec(context);
      if(status.isWorking()) {
         return status;
      }

      return this__.gotoState<Final>(context, status);
   }

   OVERRIDE(handleEvent(Procedure& this__, TransactionContext& context, const Event& event) -> Status) {
      ActionStatus status = this__.action->handleEvent(context, event);
      if(status.isWorking()) {
         return status;
      }

      return this__.gotoState<Final>(context, status);
   }
};

DEF_SINGLETON(Procedure::Final, Procedure::State) {
   OVERRIDE(enter(Procedure& this__, TransactionContext& context, Status result) -> Status) {
      this__.action = nullptr;
      return result;
   }
};

auto Procedure::exec(TransactionContext& context)                -> Status {
   if(state == nullptr) {
      auto status = gotoState<Idle>(context, Result::SUCCESS);
      if(status != Result::SUCCESS) {
         return status;
      }
   }

   return state->exec(*this, context);
}

auto Procedure::handleEvent(TransactionContext& context, const Event& event) -> Status {
   return state->handleEvent(*this, context, event);
}

auto Procedure::stop(TransactionContext&)                -> Status {
   return Result::FAILED;
}

auto Procedure::kill(TransactionContext&)                -> void {

}


TSL_NS_END