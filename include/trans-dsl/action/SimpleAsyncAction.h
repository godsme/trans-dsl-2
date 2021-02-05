//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_SIMPLEASYNCACTION_H
#define TRANS_DSL_2_SIMPLEASYNCACTION_H

#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/domain/EventId.h>
#include <trans-dsl/action/DummyAsyncAction.h>
#include <trans-dsl/action/EventHandlerRegistry.h>
#include <trans-dsl/action/MsgHandlerTrait.h>
#include <trans-dsl/action/P2MFExtractor.h>

TSL_NS_BEGIN

struct TransactionInfo;

template<typename T_REGISTRY>
struct GenericSimpleAsyncAction {
   constexpr GenericSimpleAsyncAction() {}

   auto handleEvent(TransactionInfo const& trans, Event const& event) -> Status {
      return registry.handleEvent(trans, event);
   }

   auto kill(TransactionInfo const&, Status) -> Status {
      if(registry.isWaiting()) {
         registry.reset();
         return Result::SUCCESS;
      }
      return Result::NOTHING_CHANGED;
   }

protected:
   template<typename M>
   auto waitOn(
      EventId eventId,
      M&& handler
      ) -> Status
   {
       if constexpr (MsgHandlerTrait<M>::IsNormalFunction) {
           return registry.addHandler(eventId, handler);
       } else {
           static_assert(sizeof(M) == 1);
           return registry.addHandler(eventId, extractP2MF(&M::operator()));
       }
   }

private:
   T_REGISTRY registry;
};

using SimpleAsyncAction = GenericSimpleAsyncAction<EventHandlerRegistry>;

namespace details
{
   template<typename T_ACTION>
   struct ReflexSimpleAsyncAction: SimpleAsyncAction
   {
      constexpr ReflexSimpleAsyncAction() {}
   protected:
      typedef T_ACTION ThisType;
   };
}

#define DEF_SIMPLE_ASYNC_ACTION(action) \
struct action : TSL_NS::details::ReflexSimpleAsyncAction<action>

#define WAIN_ON(eventId, handler) waitOn(eventId, handler)

#define MSG_HANDLER(msg_type) []([[maybe_unused]] TSL_NS::TransactionInfo const& trans, [[maybe_unused]] msg_type const& msg) -> TSL_NS::Status

TSL_NS_END

#endif //TRANS_DSL_2_SIMPLEASYNCACTION_H
