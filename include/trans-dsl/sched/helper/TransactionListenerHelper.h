//
// Created by Darwin Yuan on 2020/7/7.
//

#ifndef TRANS_DSL_2_TRANSACTIONLISTENERHELPER_H
#define TRANS_DSL_2_TRANSACTIONLISTENERHELPER_H

#include <cstddef>
#include <trans-dsl/sched/domain/Event.h>
#include <trans-dsl/sched/helper/AutoActionHelper.h>
#include <trans-dsl/sched/domain/TransactionListener.h>

TSL_NS_BEGIN

#define ChEcK(...) std::enable_if_t<std::is_void_v<decltype(__VA_ARGS__)>>
#define VaL(T) std::declval<T>()

namespace details {
   template<typename T, typename = void>
   constexpr bool Has_onActionStarting = false;

   template<typename T>
   constexpr bool Has_onActionStarting<T,
      ChEcK(VaL(T).onActionStarting(VaL(ActionId), VaL(TransactionInfo const&)))> = true;

   template<typename T, typename = void>
   constexpr bool Has_onActionEventConsumed = false;

   template<typename T>
   constexpr bool Has_onActionEventConsumed<T,
      ChEcK(VaL(T).onActionEventConsumed(VaL(ActionId), VaL(TransactionInfo const&), VaL(Event const&)))> = true;


   template<typename ... T_LISTENERS>
   struct UserTransListener;

   template<typename H, typename ... Ts>
   struct UserTransListener<H, Ts...> : H, UserTransListener<Ts...> {
      using Base = UserTransListener<Ts...>;
      using Listener = H;
      constexpr static bool has_onActionStarting = Has_onActionStarting<H>;
      constexpr static bool has_onActionEventConsumed = Has_onActionEventConsumed<H>;
   };

   template<>
   struct UserTransListener<> {};

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
   /////////////////////////////////////////////////////////////////////////////////
   template<typename ... T_LISTENERS>
   struct TransactionListenerDispatcher
      : UserTransListener<T_LISTENERS...>, TransactionListener {

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
         __CaLl_onActionStarting(5)
         __CaLl_onActionStarting(6)
         __CaLl_onActionStarting(7)
         __CaLl_onActionStarting(8)
         __CaLl_onActionStarting(9)
         __CaLl_onActionStarting(10)
         __CaLl_onActionStarting(11)
         __CaLl_onActionStarting(12)
         __CaLl_onActionStarting(13)
         __CaLl_onActionStarting(14)
         __CaLl_onActionStarting(15)
         __CaLl_onActionStarting(16)
         __CaLl_onActionStarting(17)
         __CaLl_onActionStarting(18)
         __CaLl_onActionStarting(19)
         __CaLl_onActionStarting(20)
      }

      #define __cAlL_onActionStarting__m(n) case n: { onActionStarting_<n>(trans); break; }
      #define __Call_onActionStarting_m(n) _SIMPLE_REPEAT(n, __cAlL_onActionStarting__m)

      OVERRIDE(onActionStarting(ActionId aid, TransactionInfo const& trans) -> void) {
         __jump_call_listener(__Call_onActionStarting_m)
      }

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
         __CaLl_onActionEventConsumed(5)
         __CaLl_onActionEventConsumed(6)
         __CaLl_onActionEventConsumed(7)
         __CaLl_onActionEventConsumed(8)
         __CaLl_onActionEventConsumed(9)
         __CaLl_onActionEventConsumed(10)
         __CaLl_onActionEventConsumed(11)
         __CaLl_onActionEventConsumed(12)
         __CaLl_onActionEventConsumed(13)
         __CaLl_onActionEventConsumed(14)
         __CaLl_onActionEventConsumed(15)
         __CaLl_onActionEventConsumed(16)
         __CaLl_onActionEventConsumed(17)
         __CaLl_onActionEventConsumed(18)
         __CaLl_onActionEventConsumed(19)
         __CaLl_onActionEventConsumed(20)
      }

      #define __cAlL_onActionEventConsumed__m(n) case n: { onActionEventConsumed_<n>(trans, ev); break; }
      #define __Call_onActionEventConsumed_m(n) _SIMPLE_REPEAT(n, __cAlL_onActionEventConsumed__m)
      OVERRIDE(onActionEventConsumed(ActionId aid, TransactionInfo const& trans, Event const& ev) -> void) {
         __jump_call_listener(__Call_onActionEventConsumed_m)
      }
   };
}
TSL_NS_END

#endif //TRANS_DSL_2_TRANSACTIONLISTENERHELPER_H
