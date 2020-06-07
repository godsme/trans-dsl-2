//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_PROCEDURE_H
#define TRANS_DSL_2_PROCEDURE_H

#include <trans-dsl/tsl_status.h>

TSL_NS_BEGIN

struct TransactionContext;

struct Procedure {
   Procedure() = default;

private:
   struct State;
   State* state = nullptr;

private:
   struct Idle;

private:
   template<typename T>
   Status gotoState(TransactionContext& context, const Status status);

};

TSL_NS_END

#endif //TRANS_DSL_2_PROCEDURE_H
