//
// Created by godsme on 2021/2/5.
//

#ifndef TRANS_DSL_2_MSGHANDLERTRAIT_H
#define TRANS_DSL_2_MSGHANDLERTRAIT_H

#include <trans-dsl/tsl_ns.h>

TSL_NS_BEGIN

struct TransactionInfo;

template<typename T>
struct MsgHandlerTrait__;

template<typename MSG_TYPE>
struct MsgHandlerTrait__<auto (TransactionInfo const&, MSG_TYPE const&) -> Status> {
    static constexpr bool IsNormalFunction = true;
};

template<typename C, typename MSG_TYPE>
struct MsgHandlerTrait__<auto (C::*)(TransactionInfo const&, MSG_TYPE const&) const -> Status> {
    static constexpr bool IsNormalFunction = false;
};

template<typename T, typename = void>
struct MsgHandlerTrait : MsgHandlerTrait__<T> {};

template<typename T>
struct MsgHandlerTrait<T, std::enable_if_t<std::is_class_v<T>>> : MsgHandlerTrait__<decltype(&T::operator())> {};

TSL_NS_END

#endif //TRANS_DSL_2_MSGHANDLERTRAIT_H
