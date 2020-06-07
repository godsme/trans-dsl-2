#include <iostream>
#include <trans-dsl/sched/helper/Sequential.h>
#include <trans-dsl/sched/concept/TransactionContext.h>

using namespace TSL_NS;

struct Action1 : SchedAction {
   auto exec(TransactionContext& context) -> Status {
      return Result::CONTINUE;
   }

   auto handleEvent(TransactionContext& context, Event& event) -> Status {
      return Result::FATAL_BUG;
   }

   auto stop(TransactionContext& context, Status cause) -> Status {
      return Result::FATAL_BUG;
   }

   auto kill(TransactionContext& context, Status cause) -> void {}
};

struct Action2 : SchedAction  {
   auto exec(TransactionContext& context) -> Status {
      return Result::UNKNOWN_EVENT;
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
   SEQUENTIAL__<Action1, Action2> a;
   std::cout << sizeof(a) << std::endl;
   auto r = a.get(1);
   if(r != nullptr) {
      std::cout << r->exec(context) << std::endl;
   } else {
      std::cout << "Hello, World!" << std::endl;
   }

   //std::cout << "Hello, World!" << std::endl;
   return 0;
}
