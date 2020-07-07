//
// Created by Darwin Yuan on 2020/7/7.
//

#ifndef TRANS_DSL_2_TRANSACTIONLISTENERHELPER_H
#define TRANS_DSL_2_TRANSACTIONLISTENERHELPER_H

#include <cstddef>
#include <trans-dsl/sched/helper/UserTransListener.h>
#include <trans-dsl/sched/domain/TransactionListener.h>
#include <trans-dsl/sched/domain/ObservedActionIdRegistry.h>

TSL_NS_BEGIN

#define __BaSe(n) Base::
#define __EnD_BaSe(n) Base
#define BaSe(n) REPEAT(n, __BaSe, __EnD_BaSe)

#define __Listener_CaLl(n, flag, expr) \
     if constexpr(BaSe(n)::has_##flag && \
        (BaSe(n)::Listener::Aids.isEnabled(AID) || BaSe(n)::Listener::Aids.isEnabled(0))) \
        {BaSe(n)::Listener::expr;}
#define __Listener_CaLl_AlL(n, macro) \
     if constexpr(Num_Of_Listeners == n) { _SIMPLE_REPEAT(n, macro); }

#define __jump_call_listener(macro) \
   switch (aid) { \
      macro(63) \
      default: break; \
   }

#define __cHeCk_CaLl(n, name, params) \
   case n: { \
     if constexpr(Base::all_observed.isEnabled(n) || Base::all_observed.isEnabled(0)) \
        name<n>params; \
     break; \
   }

/////////////////////////////////////////////////////////////////////////////////
namespace details {
   template<typename ... T_LISTENERS>
   struct TransactionListenerDispatcher
      : UserTransListener<T_LISTENERS...>, TransactionListener {
      constexpr static TransListenerObservedAids all_observed = UserTransListener<T_LISTENERS...>::all_observed;
   private:
      enum { Num_Of_Listeners = sizeof...(T_LISTENERS) };
      static_assert(Num_Of_Listeners <= 20, "too much listeners");

      using Base = UserTransListener<T_LISTENERS...>;

      //////////////////////////////////////////////////////////////////////////////////
      #define __CaLl_onActionStarting__(n) __Listener_CaLl(n, onActionStarting, onActionStarting(AID, trans))
      #define __CaLl_onActionStarting(n)  __Listener_CaLl_AlL(n, __CaLl_onActionStarting__)
      template<ActionId AID>
      auto onActionStarting_(TransactionInfo const &trans) -> void {
         __CaLl_onActionStarting(1);
         __CaLl_onActionStarting(2)
         __CaLl_onActionStarting(3)
         __CaLl_onActionStarting(4)
//         __CaLl_onActionStarting(5)
//         __CaLl_onActionStarting(6)
//         __CaLl_onActionStarting(7)
//         __CaLl_onActionStarting(8)
//         __CaLl_onActionStarting(9)
//         __CaLl_onActionStarting(10)
//         __CaLl_onActionStarting(11)
//         __CaLl_onActionStarting(12)
//         __CaLl_onActionStarting(13)
//         __CaLl_onActionStarting(14)
//         __CaLl_onActionStarting(15)
//         __CaLl_onActionStarting(16)
//         __CaLl_onActionStarting(17)
//         __CaLl_onActionStarting(18)
//         __CaLl_onActionStarting(19)
//         __CaLl_onActionStarting(20)
      }

      #define __cAlL_onActionStarting__m(n) __cHeCk_CaLl(n, onActionStarting_, (trans))
      #define __Call_onActionStarting_m(n) _SIMPLE_REPEAT(n, __cAlL_onActionStarting__m)
   public:
      OVERRIDE(onActionStarting(ActionId aid, TransactionInfo const& trans) -> void) {
         if constexpr(Base::someone_has_onActionStarting) {
            __jump_call_listener(__Call_onActionStarting_m)
         }
      }

//      //////////////////////////////////////////////////////////////////////////////////////
   private:
      //////////////////////////////////////////////////////////////////////////////////////
      #define __CaLl_onActionEventConsumed_(n) \
        __Listener_CaLl(n, onActionEventConsumed, onActionEventConsumed(AID, trans, ev))
      #define __CaLl_onActionEventConsumed(n)  __Listener_CaLl_AlL(n, __CaLl_onActionEventConsumed_)

      template<ActionId AID>
      auto onActionEventConsumed_(TransactionInfo const &trans, Event const &ev) -> void {
         __CaLl_onActionEventConsumed(1)
         __CaLl_onActionEventConsumed(2)
         __CaLl_onActionEventConsumed(3)
         __CaLl_onActionEventConsumed(4)
//         __CaLl_onActionEventConsumed(5)
//         __CaLl_onActionEventConsumed(6)
//         __CaLl_onActionEventConsumed(7)
//         __CaLl_onActionEventConsumed(8)
//         __CaLl_onActionEventConsumed(9)
//         __CaLl_onActionEventConsumed(10)
//         __CaLl_onActionEventConsumed(11)
//         __CaLl_onActionEventConsumed(12)
//         __CaLl_onActionEventConsumed(13)
//         __CaLl_onActionEventConsumed(14)
//         __CaLl_onActionEventConsumed(15)
//         __CaLl_onActionEventConsumed(16)
//         __CaLl_onActionEventConsumed(17)
//         __CaLl_onActionEventConsumed(18)
//         __CaLl_onActionEventConsumed(19)
//         __CaLl_onActionEventConsumed(20)
      }


      #define __cAlL_onActionEventConsumed__m(n) __cHeCk_CaLl(n, onActionEventConsumed_, (trans, ev))
      #define __Call_onActionEventConsumed_m(n) _SIMPLE_REPEAT(n, __cAlL_onActionEventConsumed__m)
   public:
      OVERRIDE(onActionEventConsumed(ActionId aid, TransactionInfo const& trans, Event const& ev) -> void) {
         if constexpr(Base::someone_has_onActionEventConsumed) {
            __jump_call_listener(__Call_onActionEventConsumed_m)
         }
      }

      //////////////////////////////////////////////////////////////////////////////////////
   private:
      //////////////////////////////////////////////////////////////////////////////////////
#define __CaLl_onActionDone_(n) \
        __Listener_CaLl(n, onActionDone, onActionDone(AID, trans, cause))
#define __CaLl_onActionDone(n)  __Listener_CaLl_AlL(n, __CaLl_onActionDone_)

      template<ActionId AID>
      auto onActionDone_(TransactionInfo const &trans, Status cause) -> void {
         __CaLl_onActionDone(1)
         __CaLl_onActionDone(2)
         __CaLl_onActionDone(3)
         __CaLl_onActionDone(4)
//         __CaLl_onActionDone(5)
//         __CaLl_onActionDone(6)
//         __CaLl_onActionDone(7)
//         __CaLl_onActionDone(8)
//         __CaLl_onActionDone(9)
//         __CaLl_onActionDone(10)
//         __CaLl_onActionDone(11)
//         __CaLl_onActionDone(12)
//         __CaLl_onActionDone(13)
//         __CaLl_onActionDone(14)
//         __CaLl_onActionDone(15)
//         __CaLl_onActionDone(16)
//         __CaLl_onActionDone(17)
//         __CaLl_onActionDone(18)
//         __CaLl_onActionDone(19)
//         __CaLl_onActionDone(20)
      }

      #define __cAlL_onActionDone__m(n) __cHeCk_CaLl(n, onActionDone_, (trans, cause))
      #define __cAlL_onActionDone_m(n) _SIMPLE_REPEAT(n, __cAlL_onActionDone__m)
   public:
      OVERRIDE(onActionDone(ActionId aid, TransactionInfo const& trans, Status cause) -> void) {
         if constexpr(Base::someone_has_onActionDone) {
            __jump_call_listener(__cAlL_onActionDone_m)
         }
      }
   };
}

#define __listeners(...) TSL_NS::details::TransactionListenerDispatcher<__VA_ARGS__>;

TSL_NS_END

#endif //TRANS_DSL_2_TRANSACTIONLISTENERHELPER_H
