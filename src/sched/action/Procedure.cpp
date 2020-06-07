//
// Created by Darwin Yuan on 2020/6/7.
//

#include <trans-dsl/sched/action/Procedure.h>
#include <cub/dci/Role.h>
#include <trans-dsl/utils/ActionStatus.h>

TSL_NS_BEGIN

DEFINE_ROLE(Procedure::State) {
   ABSTRACT(enter(Procedure&, TransactionContext& context) -> Status);
};

struct Procedure::Idle : Procedure::State {
   OVERRIDE(enter(Procedure& __this__, TransactionContext& context) -> Status) {
      //ActionStatus result =
   }
};

template<typename T>
Status Procedure::gotoState(TransactionContext& context, const Status status) {
   state = &T::getInstance();
   return state->enter(*this, context, status);
}


TSL_NS_END