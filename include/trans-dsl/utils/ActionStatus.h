//
// Created by Darwin Yuan on 2020/6/7.
//

#ifndef TRANS_DSL_2_ACTIONSTATUS_H
#define TRANS_DSL_2_ACTIONSTATUS_H

#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/tsl_status.h>

TSL_NS_BEGIN

//struct ActionStatus
//{
//   constexpr ActionStatus() = default;
//   constexpr ActionStatus(Status status) : status(status) {}
//
//   [[nodiscard]] constexpr auto isWorking() const -> bool {
//      return (Result::__WORKING_STATUS_BEGIN & status) > 0;
//   }
//
//   [[nodiscard]] constexpr auto isFailed() const -> bool {
//      return cub::is_failed_status(status);
//   }
//
//   [[nodiscard]] constexpr auto isDone() const -> bool {
//      return status == Result::SUCCESS;
//   }
//
//   [[nodiscard]] constexpr auto isDoneOrForceStopped() const {
//      switch (status) {
//         case Result::SUCCESS:
//         case Result::FORCE_STOPPED:
//            return true;
//         default:
//            return false;
//      }
//   }
//
//   [[nodiscard]] constexpr auto eventAccepted() const -> bool {
//      return status != Result::UNKNOWN_EVENT;
//   }
//
//   [[nodiscard]] constexpr auto eventNotAccepted() const -> bool {
//      return status == Result::UNKNOWN_EVENT;
//   }
//
//   [[nodiscard]] constexpr auto isNothingChanged() const -> bool {
//      return status == Result::NOTHING_CHANGED;
//   }
//
//   [[nodiscard]] constexpr operator Status() const {
//      return Status(status);
//   }
//
//private:
//   Status status = Result::SUCCESS;
//};

TSL_NS_END

#endif //TRANS_DSL_2_ACTIONSTATUS_H
