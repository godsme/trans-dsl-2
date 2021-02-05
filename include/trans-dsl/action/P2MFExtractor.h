//
// Created by Darwin Yuan on 2020/6/10.
//

#ifndef TRANS_DSL_2_P2MFEXTRACTOR_H
#define TRANS_DSL_2_P2MFEXTRACTOR_H

#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/domain/Event.h>
#include <trans-dsl/action/DummyAsyncAction.h>

TSL_NS_BEGIN

struct TransactionInfo;
struct DummyAsyncAction;

template<typename T, typename MSG_TYPE>
using P2MF = Status (T::*)(TransactionInfo const&, MSG_TYPE const&) const;

template<typename T, typename MSG_TYPE>
inline details::MemberFunction extractP2MF(P2MF<T, MSG_TYPE> const& p) {
   // the size of a pointer-to-mem-function is double as normal one.
   // 'coz it has to store offset of `this` pointer when multi-inheritance.
   union U{
      U(P2MF<T, MSG_TYPE> const& v) : f(v) {}

      P2MF<T, MSG_TYPE> f;
      unsigned long ll[2];
   }u{p};

   //static_assert(u.ll[1] == 0, "offset is not 0");

   return reinterpret_cast<details::MemberFunction>(u.ll[0]);
}

TSL_NS_END

#endif //TRANS_DSL_2_P2MFEXTRACTOR_H
