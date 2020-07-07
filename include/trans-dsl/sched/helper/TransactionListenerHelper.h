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
      constexpr static bool has_onActionStarting      = Has_onActionStarting<H>;
      constexpr static bool has_onActionEventConsumed = Has_onActionEventConsumed<H>;
   };

   template<>
   struct UserTransListener<> {};

   #define __BaSe(n) Base::
   #define __EnD_BaSe(n) Base
   #define BaSe(n) REPEAT(n, __BaSe, __EnD_BaSe)

   #define __Listener_CaLl(n, flag, expr) \
     if constexpr(BaSe(n)::has_##flag) {BaSe(n)::Listener::expr;}
   #define __CaLl__(n, macro)   \
   if constexpr(Num_Of_Listeners == n) { \
     _SIMPLE_REPEAT(n, macro); \
   }

   /////////////////////////////////////////////////////////////////////////////////
   template<typename ... T_LISTENERS>
   struct TransactionListenerDispatcher
      : UserTransListener<T_LISTENERS...>
      , TransactionListener {

      enum { Num_Of_Listeners = sizeof...(T_LISTENERS) };
      using Base = UserTransListener<T_LISTENERS...>;

      //////////////////////////////////////////////////////////////////////////////////
      #define __CaLl_onActionStarting__(n) __Listener_CaLl(n, onActionStarting, onActionStarting(aid, trans))
      #define __CaLl_onActionStarting(n)  __CaLl__(n, __CaLl_onActionStarting__)
      //#define __CaLl_onActionStarting(n) _SIMPLE_REPEAT(n, __CaLl_onActionStarting_)

      OVERRIDE(onActionStarting(ActionId aid, TransactionInfo const& trans) -> void) {
         __CaLl_onActionStarting(1);
         __CaLl_onActionStarting(2)
         __CaLl_onActionStarting(3)
         __CaLl_onActionStarting(4)
      }

      //////////////////////////////////////////////////////////////////////////////////////
      #define __CaLl_onActionEventConsumed_(n) \
         __Listener_CaLl(n, onActionEventConsumed, onActionEventConsumed(aid, trans, ev))
      #define __CaLl_onActionEventConsumed(n)  __CaLl__(n, __CaLl_onActionEventConsumed_)

      OVERRIDE(onActionEventConsumed(ActionId aid, TransactionInfo const& trans, Event const& ev) -> void) {
         __CaLl_onActionEventConsumed(1)
         __CaLl_onActionEventConsumed(2)
         __CaLl_onActionEventConsumed(3)
         __CaLl_onActionEventConsumed(4)
      }
   };

TSL_NS_END

#endif //TRANS_DSL_2_TRANSACTIONLISTENERHELPER_H
