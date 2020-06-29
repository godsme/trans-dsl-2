//
// Created by Darwin Yuan on 2020/6/7.
//

#include <trans-dsl/sched/action/SchedProcedure.h>
#include <cub/dci/Role.h>
#include <trans-dsl/utils/ActionStatus.h>
#include <trans-dsl/sched/domain/SchedAction.h>
#include <cub/gof/Singleton.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <trans-dsl/sched/domain/RuntimeContextAutoSwitch.h>

TSL_NS_BEGIN

#if USING_SM_PATTERN

DEFINE_ROLE(SchedProcedure::State) {
   DEFAULT(enter(SchedProcedure&, TransactionContext&, ActionStatus status) -> Status) {
      return status;
   }

   DEFAULT(exec(SchedProcedure&, TransactionContext&) -> Status) {
      return Result::FATAL_BUG;
   }

   DEFAULT(handleEvent(SchedProcedure&, TransactionContext&, Event const&) -> Status) {
      return Result::FATAL_BUG;
   }

   DEFAULT(stop(SchedProcedure&, TransactionContext&, Status cause) -> Status) {
      return Result::FATAL_BUG;
   }

   DEFAULT(kill(SchedProcedure& this_, TransactionContext& context, Status cause) -> void) {
      if(this_.action != nullptr) {
         this_.action->kill(context, cause);
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
   OVERRIDE(handleEvent(SchedProcedure& this_, TransactionContext& context, Event const& event) -> Status) {
      ActionStatus status = this_.action->handleEvent(context, event);
      if(status.isWorking()) {
         return workingStateCheck(this_, context, status);
      } else {
         return this_.gotoState<Final>(context, status);
      }
   }

   OVERRIDE(stop(SchedProcedure& this_, TransactionContext& context, Status cause) -> Status) {
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
   OVERRIDE(handleEvent(SchedProcedure& this_, TransactionContext& context, Event const& event) -> Status) {
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

   OVERRIDE(handleEvent(SchedProcedure& this_, TransactionContext& context, Event const& event) -> Status) {
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

   OVERRIDE(kill(SchedProcedure& this_, TransactionContext& context, Status) -> void) {}
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

auto SchedProcedure::handleEvent(TransactionContext& context, Event const& event) -> Status {
   AUTO_SWITCH();
   return state->handleEvent(*this, context, event);
}

auto SchedProcedure::stop(TransactionContext& context, Status cause) -> Status {
   AUTO_SWITCH();
   return state->stop(*this, context, cause);
}

auto SchedProcedure::kill(TransactionContext& context, Status cause)  -> void {
   AUTO_SWITCH();
   state->kill(*this, context, cause);
}

#else

//////////////////////////////////////////////////////////////////////////////////
#define AUTO_SWITCH()  RuntimeContextAutoSwitch autoSwitch__{context, *this}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::gotoDone(TransactionContext& context, ActionStatus status) -> Status {
   state = State::Done;

   [[unlikely]]
   if(status.isFailed()) {
      return status;
   }

   return isProtected() ? Result::SUCCESS : getStatus();
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::gotoFinal(TransactionContext& context, ActionStatus status) -> Status {
   if(action = getFinalAction(); action == nullptr) {
      return Result::FATAL_BUG;
   }

   state = State::Final;

   [[unlikely]]
   if(status.isFailed()) {
      reportFailure(status);
   }

   status = action->exec(context);
   if(status.isWorking()) {
      return status;
   }

   return gotoDone(context, status);
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::workingStateCheck() -> void {
   [[unlikely]]
   if (getStatus() != Result::SUCCESS) {
      state = State::Stopping;
   }
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::exec_(TransactionContext& context) -> Status {
   [[unlikely]]
   if(action = getAction(); action == nullptr) {
      return Result::FATAL_BUG;
   }

   ActionStatus status = action->exec(context);
   if(status.isWorking()) {
      state = State::Working;
      workingStateCheck();
      return status;
   }

   return gotoFinal(context, status);
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::exec(TransactionContext& context) -> Status {
   [[unlikely]]
   if (state != State::Idle) {
      return Result::FATAL_BUG;
   }

   Status status = attachToParent(context);
   if(status != Result::SUCCESS) {
      return status;
   }

   sandbox = isProtected();

   AUTO_SWITCH();
   return exec_(context);
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::inProgress() const -> bool {
   switch (state) {
      [[likely]]
      case State::Working:
      case State::Stopping:
      case State::Final:
         return true;
      [[unlikely]]
      default:
         return false;
   }
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::handleEvent_(TransactionContext& context, Event const& event) -> Status {
   ActionStatus status = action->handleEvent(context, event);
   if(status.isWorking()) {
      if(status == Result::CONTINUE && state == State::Working) {
         workingStateCheck();
      }
      return status;
   }

   switch (state) {
      case State::Working:
      case State::Stopping:
         return gotoFinal(context, status);
      case State::Final:
         return gotoDone(context, status);
      [[unlikely]]
      default:
         return Result::FATAL_BUG;
   }
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::handleEvent(TransactionContext& context, Event const& event) -> Status {
   [[unlikely]]
   if(!inProgress()) {
      return Result::FATAL_BUG;
   }

   AUTO_SWITCH();
   return handleEvent_(context, event);
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::stop_(TransactionContext& context, Status cause) -> Status {
   ActionStatus status = action->stop(context, cause);
   if(status.isWorking()) {
      state = State::Stopping;
      return status;
   }

   return gotoFinal(context, status);
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::stop(TransactionContext& context, Status cause) -> Status {
   switch (state) {
      [[likely]]
      case State::Working:
         break;
      case State::Stopping:
      case State::Final:
         return Result::CONTINUE;
      [[unlikely]]
      default:
         return FATAL_BUG;
   }

   AUTO_SWITCH();
   return stop_(context, cause);
}

//////////////////////////////////////////////////////////////////////////////////
auto SchedProcedure::kill(TransactionContext& context, Status cause)  -> void {
   if(inProgress()) {
      AUTO_SWITCH();
      action->kill(context, cause);
      action = nullptr;
      state = State::Done;
   }
}

#endif

TSL_NS_END