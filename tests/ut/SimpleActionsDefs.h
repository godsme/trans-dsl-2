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
   const TSL_NS::EventId EV_MSG_1 = 1;
   const TSL_NS::EventId EV_MSG_2 = 2;
   struct Msg1 {
      int a, b;
   };

   struct Msg2 {
      int a;
   };

   DEF_SIMPLE_ASYNC_ACTION(AsyncAction1) {
      auto exec(const TSL_NS::TransactionInfo &) -> TSL_NS::Status {
         return WAIN_ON(EV_MSG_1, handleMsg1);
      }

   DEF_INLINE_EVENT_HANDLER(handleMsg1, Msg1) {
         return TSL_NS::Result::SUCCESS;
      }
   };

   DEF_SIMPLE_ASYNC_ACTION(AsyncAction2) {
      auto exec(const TSL_NS::TransactionInfo &) -> TSL_NS::Status {
         return WAIN_ON(EV_MSG_2, handleMsg2);
      }

   DEF_INLINE_EVENT_HANDLER(handleMsg2, Msg2) {
         return TSL_NS::Result::SUCCESS;
      }
   };

   const auto SyncAction1 = [](const TSL_NS::TransactionInfo&) -> TSL_NS::Status {
      return TSL_NS::Result::SUCCESS;
   };

   inline auto SyncAction2(const TSL_NS::TransactionInfo&) -> TSL_NS::Status {
      return TSL_NS::Result::SUCCESS;
   }

   struct SyncAction3 {
      auto exec(const TSL_NS::TransactionInfo&) -> TSL_NS::Status {
         return result;
      }

   private:
      TSL_NS::Status result = TSL_NS::Result::SUCCESS;
   };



}

#endif //TRANS_DSL_2_SIMPLEACTIONSDEFS_H
