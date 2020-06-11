//
// Created by Darwin Yuan on 2020/6/7.
//

#include <trans-dsl/sched/action/SchedProcedure.h>
#include <cub/dci/Role.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/concept/SchedAction.h>
#include <cub/gof/Singleton.h>

TSL_NS_BEGIN

DEFINE_ROLE(SchedProcedure::State) {
   DEFAULT(enter(SchedProcedure&, TransactionContext&, Status status) -> Status) {
      return status;
   }

   DEFAULT(exec(SchedProcedure&, TransactionContext&) -> Status) {
      return Result::FATAL_BUG;
   }

   DEFAULT(handleEvent(SchedProcedure&, TransactionContext&, const Event&) -> Status) {
      return Result::FATAL_BUG;
   }

   DEFAULT(stop(SchedProcedure&, TransactionContext&) -> Status) {
      return Result::FATAL_BUG;
   }

   DEFAULT(kill(SchedProcedure& this__, TransactionContext& context) -> void) {
      if(this__.action != nullptr) {
         this__.action->kill(context);
         (void) this__.gotoState<Done>(context, Result::SUCCESS);
      }
   }
};

#define DEF_STATE(state) DEF_SINGLETON(SchedProcedure::state, SchedProcedure::State)

template<typename T>
inline Status SchedProcedure::gotoState(TransactionContext& context, Status status) {
   state = &T::getInstance();
   return state->enter(*this, context, status);
}

DEF_STATE(Idle)  {
   OVERRIDE(enter(SchedProcedure& this__, TransactionContext&, Status status) -> Status) {
      if(this__.action = this__.getAction(); this__.action == nullptr) {
         return Result::FATAL_BUG;
      }

      return status;
   }

   OVERRIDE(exec(SchedProcedure& this__, TransactionContext& context) -> Status) {
      auto status = this__.action->exec(context);
      if(status == Result::CONTINUE) {
         return this__.gotoState<Working>(context, status);
      } else {
         return this__.gotoState<Final>(context, status);
      }
   }
};

DEF_STATE(Working) {
   OVERRIDE(handleEvent(SchedProcedure& this__, TransactionContext& context, const Event& event) -> Status) {
      ActionStatus status = this__.action->handleEvent(context, event);
      if(status.isWorking()) {
         return status;
      } else {
         return this__.gotoState<Final>(context, status);
      }
   }

   DEFAULT(stop(SchedProcedure& this__, TransactionContext& context) -> Status) {
      ActionStatus status = this__.action->stop(context);
      if(status.isWorking()) {
         return this__.gotoState<Stopping>(context, status);
      } else {
         return this__.gotoState<Final>(context, status);
      }
   }
};

DEF_STATE(Stopping) {
   OVERRIDE(handleEvent(SchedProcedure& this__, TransactionContext& context, const Event& event) -> Status) {
      ActionStatus status = this__.action->handleEvent(context, event);
      if(status.isWorking()) {
         return status;
      } else {
         return this__.gotoState<Final>(context, status);
      }
   }

   OVERRIDE(stop(SchedProcedure& this__, TransactionContext& context) -> Status) {
      return Result::CONTINUE;
   }
};

DEF_STATE(Final) {
   OVERRIDE(enter(SchedProcedure& this__, TransactionContext& context, Status result) -> Status) {
      if(this__.action = this__.getFinalAction(); this__.action == nullptr) {
         return Result::FATAL_BUG;
      }

      ActionStatus status = this__.action->exec(context);
      if(status.isWorking()) {
         return status;
      }

      return this__.gotoState<Done>(context, status);
   }

   OVERRIDE(handleEvent(SchedProcedure& this__, TransactionContext& context, const Event& event) -> Status) {
      ActionStatus status = this__.action->handleEvent(context, event);
      if(status.isWorking()) {
         return status;
      }

      return this__.gotoState<Done>(context, status);
   }

   OVERRIDE(stop(SchedProcedure& this__, TransactionContext& context) -> Status) {
      return Result::CONTINUE;
   }
};

DEF_STATE(Done) {
   OVERRIDE(enter(SchedProcedure& this__, TransactionContext& context, Status result) -> Status) {
      this__.action = nullptr;
      return result;
   }

   OVERRIDE(kill(SchedProcedure& this__, TransactionContext& context) -> void) {}
};

auto SchedProcedure::exec(TransactionContext& context)                -> Status {
   if(state == nullptr) {
      auto status = gotoState<Idle>(context, Result::SUCCESS);
      if(status != Result::SUCCESS) {
         return status;
      }
   }

   return state->exec(*this, context);
}

auto SchedProcedure::handleEvent(TransactionContext& context, const Event& event) -> Status {
   return state->handleEvent(*this, context, event);
}

auto SchedProcedure::stop(TransactionContext& context) -> Status {
   return state->stop(*this, context);
}

auto SchedProcedure::kill(TransactionContext& context)  -> void {
   state->kill(*this, context);
}


TSL_NS_END