//
// Created by Darwin Yuan on 2020/7/4.
//

#ifndef TRANS_DSL_2_TRANSACTIONHELPER_H
#define TRANS_DSL_2_TRANSACTIONHELPER_H

#include <trans-dsl/sched/helper/MultiThreadHelper.h>
#include <trans-dsl/sched/domain/MultiThreadContext.h>
#include <trans-dsl/sched/domain/TransactionContext.h>
#include <cstddef>
#include <trans-dsl/sched/domain/Event.h>
#include <trans-dsl/sched/helper/AutoActionHelper.h>
#include <trans-dsl/sched/helper/ActionRealTypeTraits.h>

TSL_NS_BEGIN

namespace details {
   template<typename ACTION>
   struct Transaction {
   private:
      template<typename T, typename = void>
      struct MultiThreadTrait {
         using type = __multi_thread(T);
      };

      template<typename T>
      struct MultiThreadTrait<T, std::enable_if_t<std::is_void_v<ThreadCreator_t<T>>>> {
         using type = T;
      };

   public:
      template<TransListenerObservedAids const& AIDs>
      class ActionRealType : private TransactionContext {
      public:
         using RawType = ActionRealTypeTraits_t<AIDs, ACTION>;
         using TraitsType = typename MultiThreadTrait<RawType>::type;
         using Action = ActionRealTypeTraits_t<AIDs, TraitsType>;

         using Outer = Transaction<ACTION>;

         using TransactionContext::updateInstanceId;
         using TransactionContext::updateUserContext;
         using TransactionContext::updateTimerInfo;

         auto start() -> Status {
            return action.exec(*this);
         }

         auto startWithEvent(Event const& event) -> Status {
            auto status = action.exec(*this);
            if(status != Result::CONTINUE) return status;
            return action.handleEvent(*this, event);
         }

         auto handleEvent(Event const& event) -> Status {
            return action.handleEvent(*this, event);
         }

         auto stop(Status cause) -> Status {
            return action.stop(*this, cause);
         }

         auto kill(Status cause) {
            action.kill(*this, cause);
         }

      private:
         Action action;

      protected:
         using TransactionContext::updateListener;
      };
   };

   template<typename ACTION>
   using Transaction_t = typename Transaction<ACTION>::template ActionRealType<EmptyAids>;

   /////////////////////////////////////////////////////////
   template<typename TRANSACTION, typename LISTENER>
   class TransObserverBinder final {
      //constexpr static TransListenerObservedAids observedAids = LISTENER::all_observed;
      using Trans = typename TRANSACTION::Outer::template ActionRealType<LISTENER::all_observed>;
      //static_assert(sizeof(Trans) > sizeof(TRANSACTION));

   public:
      struct Inner : private LISTENER, Trans {
         using RawType = typename Trans::RawType;
         using Trans::updateInstanceId;
         using Trans::updateUserContext;
         using Trans::updateTimerInfo;

         using Trans::start;
         using Trans::startWithEvent;
         using Trans::handleEvent;
         using Trans::stop;
         using Trans::kill;

         Inner() {
            Trans::updateListener(static_cast<TransactionListener&>(*this));
         }
      };
   };
}

TSL_NS_END

#define __transaction(...) \
  TSL_NS::details::Transaction_t<TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>>

#define __as_trans(...) __transaction(__VA_ARGS__)

#define __bind_listener(trans, listeners) \
  typename TSL_NS::details::TransObserverBinder<trans, listeners>::Inner;

#endif //TRANS_DSL_2_TRANSACTIONHELPER_H
