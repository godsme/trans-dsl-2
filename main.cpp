#include <iostream>
#include <trans-dsl/sched/helper/SequentialHelper.h>
#include <trans-dsl/sched/concept/TransactionContext.h>
#include <event/concept/Event.h>
#include <event/impl/ConsecutiveEventInfo.h>
#include <trans-dsl/sched/helper/SyncActionHelper.h>
#include <trans-dsl/sched/helper/AsyncActionHelper.h>
#include <trans-dsl/action/TransactionInfo.h>
#include <trans-dsl/action/SimpleAsyncAction.h>
#include <trans-dsl/action/P2MFExtractor.h>

using namespace TSL_NS;

struct Action1 : SchedAction {
   auto exec(TransactionContext& context) -> Status {
      return Result::CONTINUE;
   }

   auto handleEvent(TransactionContext& context, Event& event) -> Status {
      return Result::SUCCESS;
   }

   auto stop(TransactionContext& context, Status cause) -> Status {
      return Result::FATAL_BUG;
   }

   auto kill(TransactionContext& context, Status cause) -> void {}
};

struct Action2 : SchedAction  {
   auto exec(TransactionContext& context) -> Status {
      return Result::SUCCESS;
   }

   auto handleEvent(TransactionContext& context, Event& event) -> Status {
      return Result::SUCCESS;
   }

   auto stop(TransactionContext& context, Status cause) -> Status {
      return Result::FATAL_BUG;
   }

   auto kill(TransactionContext& context, Status cause) -> void {}
};

struct StupidTransactionContext : private TransactionInfo, TransactionContext {
   OVERRIDE(getInstanceId() const -> InstanceId) {
      return 0;
   }
   OVERRIDE(getStatus() const -> Status) {
      return Result::SUCCESS;
   }
   OVERRIDE(getUserContext() const -> Unknown*) {
      return nullptr;
   }

private:
   IMPL_ROLE(TransactionInfo);
};


auto action2(const TransactionInfo&) -> Status {
   return Result::FAILED;
}

template<Result V_RESULT>
struct GAction {
   Status exec(const TransactionInfo&) {
      return result;
   }
   Result result = Result(V_RESULT + 1);
};

GAction<Result::SUCCESS> action3;

inline auto action1 = [](const TransactionInfo&) ->Status { return Result::FATAL_BUG; };

const EventId MSG_1 = 1000;
struct Msg1 { int a; int b; };

DEF_SIMPLE_ASYNC_ACTION(AsyncAction1) {
   auto exec(const TransactionInfo&) {
      auto r = extractP2MF(&AsyncAction1::handleMsg1);
      return WAIN_ON(MSG_1, handleMsg1);
   }

   DECL_EVENT_HANDLER(handleMsg1, Msg1);
};

DEF_EVENT_HANDLER(AsyncAction1, handleMsg1, Msg1) {
   std::cout << msg.a << " " << msg.b << " " << std::endl;
   return Result::SUCCESS;
}

int main() {
   StupidTransactionContext context{};
   Msg1 msg1 {10, 20};
   ev::ConsecutiveEventInfo info{MSG_1, msg1};
   Event event(info);

   __call(action1) b;
   __call(action2) c;

   __async(AsyncAction1) d;

   std::cout << sizeof(EventHandlerRegistry) << std::endl;
   std::cout << sizeof(c) << std::endl;
   std::cout << sizeof(d) << std::endl;

   std::cout << (d.exec(context) == Result::CONTINUE) << std::endl;
   std::cout << d.handleEvent(context, event) << std::endl;

//   __sequential(Action1, Action2, __call(action1)) a;
//
//   std::cout << b.exec(context) << std::endl;
//   std::cout << c.exec(context) << std::endl;
//
//   //auto r = a.exec(context);
//   std::cout << sizeof(a) << std::endl;
//   std::cout << a.exec(context)  << std::endl;
//   std::cout << a.handleEvent(context, event)  << std::endl;

   //std::cout << "Hello, World!" << std::endl;
   return 0;
}
