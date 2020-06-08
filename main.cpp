#include <iostream>
#include <trans-dsl/sched/helper/Sequential.h>
#include <trans-dsl/sched/concept/TransactionContext.h>
#include <event/concept/Event.h>

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
      return Result::FATAL_BUG;
   }

   auto stop(TransactionContext& context, Status cause) -> Status {
      return Result::FATAL_BUG;
   }

   auto kill(TransactionContext& context, Status cause) -> void {}
};

int main() {
   TransactionContext context{};
   Event event;

   __sequential(Action1, Action2) a;

   //auto r = a.exec(context);
   std::cout << sizeof(a) << std::endl;
   std::cout << a.exec(context)  << std::endl;
   std::cout << a.handleEvent(context, event)  << std::endl;

   //std::cout << "Hello, World!" << std::endl;
   return 0;
}
