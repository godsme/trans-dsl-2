//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_PROCEDURE_H
#define TRANS_DSL_2_PROCEDURE_H

#include <trans-dsl/tsl_status.h>
#include <cub/dci/Role.h>

TSL_NS_BEGIN

struct TransactionContext;
struct SchedAction;
struct FinalAction;

struct Procedure {
   Procedure() = default;

private:
   struct State;
   State* state = nullptr;

private:
   struct Idle;
   struct Working;
   struct Final;

private:
   template<typename T>
   Status gotoState(TransactionContext& context, Status status);

private:
   USE_ROLE(SchedAction);
   USE_ROLE(FinalAction);
};

TSL_NS_END

#endif //TRANS_DSL_2_PROCEDURE_H
