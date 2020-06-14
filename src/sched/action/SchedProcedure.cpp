//
// Created by Darwin Yuan on 2020/6/7.
//

#include <trans-dsl/sched/action/SchedProcedure.h>
#include <cub/dci/Role.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/concept/SchedAction.h>
#include <cub/gof/Singleton.h>
#include <trans-dsl/sched/concept/TransactionContext.h>
#include <trans-dsl/sched/concept/RuntimeContextAutoSwitch.h>

TSL_NS_BEGIN

DEFINE_ROLE(SchedProcedure::State) {
   DEFAULT(enter(SchedProcedure&, TransactionContext&, ActionStatus status) -> Status) {
      return status;
   }

   DEFAULT(exec(SchedProcedure&, TransactionContext&) -> Status) {
      return Result::FATAL_BUG;
   }

   DEFAULT(handleEvent(SchedProcedure&, TransactionContext&, const Event&) -> Status) {
      return Result::FATAL_BUG;
   }

   DEFAULT(stop(SchedProcedure&, TransactionContext&, Status cause) -> Status) {
      return Result::FATAL_BUG;
   }

   DEFAULT(kill(SchedProcedure& this_, TransactionContext& context) -> void) {
      if(this_.action != nullptr) {
         this_.action->kill(context);
         (void) this_.gotoState<Done>(context, Result::SUCCESS);
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
   OVERRIDE(enter(SchedProcedure& this_, TransactionContext&, ActionStatus status) -> Status) {
      if(this_.action = this_.getAction(); this_.action == nullptr) {
         return Result::FATAL_BUG;
      }

      return status;
   }

   OVERRIDE(exec(SchedProcedure& this_, TransactionContext& context) -> Status) {
      auto status = this_.action->exec(context);
      if(status == Result::CONTINUE) {
         return this_.gotoState<Working>(context, status);
      } else {
         return this_.gotoState<Final>(context, status);
      }
   }
};

DEF_STATE(Working) {
   OVERRIDE(handleEvent(SchedProcedure& this_, TransactionContext& context, const Event& event) -> Status) {
      ActionStatus status = this_.action->handleEvent(context, event);
      if(status.isWorking()) {
         return workingStateCheck(this_, context, status);
      } else {
         return this_.gotoState<Final>(context, status);
      }
   }

   OVERRIDE(stop(SchedProcedure& this_, TransactionContext& context, Status cause) -> Status) {
      this_.syncParentFailure();

      ActionStatus status = this_.action->stop(context, cause);
      if(status.isWorking()) {
         return this_.gotoState<Stopping>(context, status);
      } else {
         return this_.gotoState<Final>(context, status);
      }
   }

private:
   auto workingStateCheck(SchedProcedure& this_, TransactionContext& context, Status status) -> Status {
      if(this_.getStatus() == Result::SUCCESS) return status;
      return this_.gotoState<Stopping>(context, status);
   }
};

DEF_STATE(Stopping) {
   OVERRIDE(handleEvent(SchedProcedure& this_, TransactionContext& context, const Event& event) -> Status) {
      ActionStatus status = this_.action->handleEvent(context, event);
      if(status.isWorking()) {
         return status;
      } else {
         return this_.gotoState<Final>(context, status);
      }
   }

   OVERRIDE(stop(SchedProcedure& this_, TransactionContext& context, Status cause) -> Status) {
      return Result::CONTINUE;
   }
};

DEF_STATE(Final) {
   OVERRIDE(enter(SchedProcedure& this_, TransactionContext& context, ActionStatus result) -> Status) {
      if(this_.action = this_.getFinalAction(); this_.action == nullptr) {
         return Result::FATAL_BUG;
      }

      if(result.isFailed()) {
         this_.reportFailure(result);
      }

      ActionStatus status = this_.action->exec(context);
      if(status.isWorking()) {
         return status;
      }

      return this_.gotoState<Done>(context, status);
   }

   OVERRIDE(handleEvent(SchedProcedure& this_, TransactionContext& context, const Event& event) -> Status) {
      ActionStatus status = this_.action->handleEvent(context, event);
      if(status.isWorking()) {
         return status;
      }

      return this_.gotoState<Done>(context, status);
   }

   OVERRIDE(stop(SchedProcedure& this_, TransactionContext& context, Status cause) -> Status) {
      return Result::CONTINUE;
   }
};

DEF_STATE(Done) {
   OVERRIDE(enter(SchedProcedure& this_, TransactionContext& context, ActionStatus result) -> Status) {
      this_.action = nullptr;
      return (result == SUCCESS && !this_.isProtected()) ? this_.getStatus() : (Status)result;
   }

   OVERRIDE(kill(SchedProcedure& this_, TransactionContext& context) -> void) {}
};

auto SchedProcedure::exec_(TransactionContext& context) -> Status {
   if (state != nullptr) {
      return Result::FATAL_BUG;
   }

   auto status = gotoState<Idle>(context, Result::SUCCESS);
   if (status != Result::SUCCESS) {
      return status;
   }

   return state->exec(*this, context);
}

#define AUTO_SWITCH()  RuntimeContextAutoSwitch autoSwitch__{context, *this}

auto SchedProcedure::exec(TransactionContext& context) -> Status {
   Status status = attachToParent(context);
   if(status != Result::SUCCESS) {
      return status;
   }

   sandbox = isProtected();

   AUTO_SWITCH();
   return exec_(context);
}

auto SchedProcedure::handleEvent(TransactionContext& context, const Event& event) -> Status {
   AUTO_SWITCH();
   return state->handleEvent(*this, context, event);
}

auto SchedProcedure::stop(TransactionContext& context, Status cause) -> Status {
   AUTO_SWITCH();
   return state->stop(*this, context, cause);
}

auto SchedProcedure::kill(TransactionContext& context)  -> void {
   AUTO_SWITCH();
   state->kill(*this, context);
}


TSL_NS_END