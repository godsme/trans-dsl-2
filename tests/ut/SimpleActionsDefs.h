//
// Created by Darwin Yuan on 2020/6/10.
//

#ifndef TRANS_DSL_2_SIMPLEACTIONSDEFS_H
#define TRANS_DSL_2_SIMPLEACTIONSDEFS_H

#include <trans-dsl/action/SimpleAsyncAction.h>
#include <event/impl/ConsecutiveEventInfo.h>
#include <event/impl/SimpleEventInfo.h>
#include <event/concept/Event.h>

namespace {
   TSL_NS::TimerId constexpr TIMER_1 = 1;
   TSL_NS::TimerId constexpr TIMER_2 = 2;
   TSL_NS::EventId constexpr TIMER_EVENT_ID_1 = 400 + 1;
   TSL_NS::EventId constexpr TIMER_EVENT_ID_2 = 400 + 2;

   TSL_NS::EventId const EV_MSG_1 = 101;
   TSL_NS::EventId const EV_MSG_2 = 102;
   TSL_NS::EventId const EV_MSG_3 = 103;
   TSL_NS::EventId const EV_MSG_4 = 104;
   struct Msg1 {
      int a, b;
   };

   struct Msg2 {
      int a;
   };

   struct Msg3 {
      int a;
   };

   struct Msg4 {
      int a;
   };

   DEF_SIMPLE_ASYNC_ACTION(AsyncAction1) {
      auto exec(TSL_NS::TransactionInfo const&) -> TSL_NS::Status {
         return WAIN_ON(EV_MSG_1, [](TSL_NS::TransactionInfo const&, Msg1 const&) -> TSL_NS::Status {
             return TSL_NS::Result::SUCCESS;
         });
      }
   };

   DEF_SIMPLE_ASYNC_ACTION(AsyncAction2) {
      auto exec(TSL_NS::TransactionInfo const&) -> TSL_NS::Status {
         return WAIN_ON(EV_MSG_2, [](TSL_NS::TransactionInfo const&, Msg2 const&) -> TSL_NS::Status {
             return TSL_NS::Result::SUCCESS;
         });
      }
   };

   DEF_SIMPLE_ASYNC_ACTION(FailedAsyncAction3) {
      auto exec(TSL_NS::TransactionInfo const&) -> TSL_NS::Status {
         return WAIN_ON(EV_MSG_3, [](TSL_NS::TransactionInfo const&, Msg3 const&) -> TSL_NS::Status {
             return TSL_NS::Result::FAILED;
         });
      }
   };

   DEF_SIMPLE_ASYNC_ACTION(AsyncAction4) {
      auto exec(TSL_NS::TransactionInfo const&) -> TSL_NS::Status {
         return WAIN_ON(EV_MSG_4, [](TSL_NS::TransactionInfo const&, Msg4 const&) -> TSL_NS::Status {
             return TSL_NS::Result::SUCCESS;
         });
      }
   };

   const auto SyncAction1 = [](TSL_NS::TransactionInfo const&) -> TSL_NS::Status {
      return TSL_NS::Result::SUCCESS;
   };

   inline auto SyncAction2(TSL_NS::TransactionInfo const&) -> TSL_NS::Status {
      return TSL_NS::Result::SUCCESS;
   }

   struct SyncAction3 {
      auto operator()(TSL_NS::TransactionInfo const&) -> TSL_NS::Status {
         return result;
      }

   private:
      TSL_NS::Status result = TSL_NS::Result::SUCCESS;
   };

   inline auto FailedSyncAction4(TSL_NS::TransactionInfo const&) -> TSL_NS::Status {
      return TSL_NS::Result::FAILED;
   }

}

#endif //TRANS_DSL_2_SIMPLEACTIONSDEFS_H
