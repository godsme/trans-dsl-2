//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_TRANSACTIONINFO_H
#define TRANS_DSL_2_TRANSACTIONINFO_H

#include <trans-dsl/sched/concept/Unknown.h>
#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/concept/InstanceId.h>

TSL_NS_BEGIN

DEFINE_ROLE(TransactionInfo)
{
   ABSTRACT(getInstanceId() const -> InstanceId);
   ABSTRACT(getStatus() const -> Status);
   ABSTRACT(getUserContext() const -> Unknown*);

   template <typename ROLE>
   ROLE* toRole() const
   {
      return dynamic_cast<ROLE*>(getUserContext());
   }
};

TSL_NS_END

#endif //TRANS_DSL_2_TRANSACTIONINFO_H
