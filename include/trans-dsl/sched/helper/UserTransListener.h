//
// Created by Darwin Yuan on 2020/7/7.
//

#ifndef TRANS_DSL_2_USERTRANSLISTENER_H
#define TRANS_DSL_2_USERTRANSLISTENER_H

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

   template<typename T, typename = void>
   constexpr bool Has_onActionDone = false;

   template<typename T>
   constexpr bool Has_onActionDone<T,
      ChEcK(VaL(T).onActionDone(VaL(ActionId), VaL(TransactionInfo const&), VaL(Status)))> = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
namespace details {
   template<typename ... T_LISTENERS>
   struct UserTransListener;

   template<typename H, typename ... Ts>
   struct UserTransListener<H, Ts...> : H, UserTransListener<Ts...> {
      using Base = UserTransListener<Ts...>;
      using Listener = H;
      constexpr static bool has_onActionStarting      = Has_onActionStarting<H>;
      constexpr static bool has_onActionEventConsumed = Has_onActionEventConsumed<H>;
      constexpr static bool has_onActionDone          = Has_onActionDone<H>;

      constexpr static bool someone_has_onActionStarting      = has_onActionStarting | Base::someone_has_onActionStarting;
      constexpr static bool someone_has_onActionEventConsumed = has_onActionEventConsumed | Base::someone_has_onActionEventConsumed;
      constexpr static bool someone_has_onActionDone          = has_onActionDone | Base::someone_has_onActionDone;
   };

   template<>
   struct UserTransListener<> {
      constexpr static bool someone_has_onActionStarting = false;
      constexpr static bool someone_has_onActionEventConsumed = false;
      constexpr static bool someone_has_onActionDone = false;
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_USERTRANSLISTENER_H
