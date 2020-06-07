//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_ACTIONSTATUS_H
#define TRANS_DSL_2_ACTIONSTATUS_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/tsl_status.h>

TSL_NS_BEGIN

struct ActionStatus
{
   ActionStatus(Status status) : status(status) {}

   auto isWorking() const -> bool {
      return status == Result::CONTINUE || status == Result::UNKNOWN_EVENT;
   }

   auto isFailed() const -> bool {
      return cub::is_failed_status(cub::Status(status));
   }

   auto isDone() const -> bool {
      return status == Result::SUCCESS;
   }

   auto eventAccepted() const -> bool {
      return status != Result::UNKNOWN_EVENT;
   }

   auto eventNotAccepted() const -> bool {
      return !eventAccepted();
   }

   auto isNothingChanged() const -> bool {
      return status == Result::NOTHING_CHANGED;
   }

   operator Status() const {
      return Status(status);
   }

private:
   Status status;
};

TSL_NS_END

#endif //TRANS_DSL_2_ACTIONSTATUS_H
