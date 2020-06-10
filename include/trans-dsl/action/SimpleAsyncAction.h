//
// Created by Darwin Yuan on 2020/6/9.
//

#ifndef TRANS_DSL_2_SIMPLEASYNCACTION_H
#define TRANS_DSL_2_SIMPLEASYNCACTION_H

#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/concept/EventId.h>
#include <trans-dsl/action/DummyAsyncAction.h>
#include <trans-dsl/action/EventHandlerRegistry.h>
#include <trans-dsl/action/P2MFExtractor.h>

TSL_NS_BEGIN

struct TransactionInfo;

template<typename T_REGISTRY>
struct GenericSimpleAsyncAction {
   auto handleEvent(const TransactionInfo& trans, const Event& event) -> Status {
      return registry.handleEvent(reinterpret_cast<details::DummyAsyncAction*>(this), trans, event);
   }

   auto kill(const TransactionInfo& trans) {
      registry.reset();
   }

protected:
   template<typename T>
   auto waitOn(
      T* thisPointer,
      EventId eventId,
      const P2MF<T>& handler
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
auto handler(const TSL_NS::TransactionInfo& trans, const TSL_NS::Event& event) -> TSL_NS::Status { \
   return handler__(trans, *static_cast<const msgType*>(event.getMsg()));  \
}                                                                          \
auto handler__(const TSL_NS::TransactionInfo& trans, const msgType& msg) -> TSL_NS::Status

TSL_NS_END

#endif //TRANS_DSL_2_SIMPLEASYNCACTION_H
