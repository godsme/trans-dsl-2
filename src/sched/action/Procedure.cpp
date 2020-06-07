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
      return Result::SUCCESS;
   }
};

template<typename T>
inline Status Procedure::gotoState(TransactionContext& context, Status status) {
   state = &T::getInstance();
   return state->enter(*this, context, status);
}

DEF_SINGLETON(Procedure::Idle, Procedure::State)  {
   OVERRIDE(exec(Procedure& __this__, TransactionContext& context) -> Status) {
      ActionStatus status = __this__.ROLE(SchedAction).exec(context);
      if(status.isWorking()) {
         return __this__.gotoState<Working>(context, status);
      } else {
         return __this__.gotoState<Final>(context, status);
      }
   }
};

DEF_SINGLETON(Procedure::Working, Procedure::State) {
};

DEF_SINGLETON(Procedure::Final, Procedure::State) {
};




TSL_NS_END