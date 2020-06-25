//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_SIMPLEASYNCACTION_H
#define TRANS_DSL_2_SIMPLEASYNCACTION_H

#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/domain/EventId.h>
#include <trans-dsl/action/DummyAsyncAction.h>
#include <trans-dsl/action/EventHandlerRegistry.h>
#include <trans-dsl/action/P2MFExtractor.h>

TSL_NS_BEGIN

struct TransactionInfo;

template<typename T_REGISTRY>
struct GenericSimpleAsyncAction {
   constexpr GenericSimpleAsyncAction() {}

   auto handleEvent(TransactionInfo const& trans, Event const& event) -> Status {
      return registry.handleEvent(reinterpret_cast<details::DummyAsyncAction*>(this), trans, event);
   }

   auto kill(TransactionInfo const& trans, Status cause) -> Status {
      if(registry.isWaiting()) {
         registry.reset();
         return Result::SUCCESS;
      }
      return Result::NOTHING_CHANGED;
   }

protected:
   template<typename T>
   auto waitOn(
      T* thisPointer,
      EventId eventId,
      P2MF<T> const& handler
      ) -> Status
   {
      return registry.addHandler(eventId, extractP2MF(handler));
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

#define WAIN_ON(eventId, handler) waitOn(this, eventId, &ThisType::handler)

#define DECL_EVENT_HANDLER(handler, msgType)                      \
private:                                                          \
auto handler__(const TransactionInfo&, const msgType&) -> Status; \
auto handler(const TransactionInfo&, const Event&) -> Status

#define DEF_EVENT_HANDLER(cls, handler, msgType) \
auto cls::handler(const TransactionInfo& trans, const Event& event) -> Status { \
   return handler__(trans, *static_cast<const msgType*>(event.getMsg()));       \
}                                                                               \
auto cls::handler__(const TransactionInfo& trans, const msgType& msg) -> Status

#define DEF_INLINE_EVENT_HANDLER(handler, msgType)                         \
private:                                                                   \
auto handler(TSL_NS::TransactionInfo const& trans, TSL_NS::Event const& event) -> TSL_NS::Status { \
   return handler__(trans, *static_cast<const msgType*>(event.getMsg()));  \
}                                                                          \
auto handler__(TSL_NS::TransactionInfo const& trans, msgType const& msg) -> TSL_NS::Status

TSL_NS_END

#endif //TRANS_DSL_2_SIMPLEASYNCACTION_H
