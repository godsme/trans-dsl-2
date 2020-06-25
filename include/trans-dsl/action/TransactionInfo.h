//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_TRANSACTIONINFO_H
#define TRANS_DSL_2_TRANSACTIONINFO_H

#include <trans-dsl/sched/domain/Unknown.h>
#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/domain/InstanceId.h>

TSL_NS_BEGIN

DEFINE_ROLE(TransactionInfo)
{
   auto getInstanceId() const  -> InstanceId { return instanceId; }
   auto getUserContext() const -> Unknown* { return userContext; }

   ABSTRACT(getStatus() const -> Status);

   template <typename ROLE>
   ROLE* toRole() const
   {
      return dynamic_cast<ROLE*>(getUserContext());
   }

protected:
   TransactionInfo() = default;

private:
   TransactionInfo(TransactionInfo const&) = delete;
   TransactionInfo(TransactionInfo&&) = delete;
   TransactionInfo& operator=(TransactionInfo const&) = delete;
   TransactionInfo& operator=(TransactionInfo&&) = delete;

protected:
   auto updateInstanceId(InstanceId id) -> void { instanceId = id; }
   auto updateUserContext(Unknown* context) -> void { userContext = context; }

private:
   InstanceId instanceId = 0;
   Unknown* userContext = nullptr;
};

TSL_NS_END

#endif //TRANS_DSL_2_TRANSACTIONINFO_H
