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
   constexpr ActionStatus() = default;
   constexpr ActionStatus(Status status) : status(status) {}

   constexpr auto isWorking() const -> bool {
      return status == Result::CONTINUE || status == Result::UNKNOWN_EVENT;
   }

   constexpr auto isFailed() const -> bool {
      return cub::is_failed_status(cub::Status(status));
   }

   constexpr auto isDone() const -> bool {
      return status == Result::SUCCESS;
   }

   constexpr auto eventAccepted() const -> bool {
      return status != Result::UNKNOWN_EVENT;
   }

   constexpr auto eventNotAccepted() const -> bool {
      return !eventAccepted();
   }

   constexpr auto isNothingChanged() const -> bool {
      return status == Result::NOTHING_CHANGED;
   }

   constexpr operator Status() const {
      return Status(status);
   }

private:
   Status status = Result::SUCCESS;
};

TSL_NS_END

#endif //TRANS_DSL_2_ACTIONSTATUS_H
