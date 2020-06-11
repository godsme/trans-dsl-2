//
// Created by Darwin Yuan on 2020/6/11.
//

#ifndef TRANS_DSL_2_PROCEDUREHELPER_H
#define TRANS_DSL_2_PROCEDUREHELPER_H

#include <trans-dsl/sched/concept/TransactionContext.h>
#include <trans-dsl/sched/action/SchedProcedure.h>
#include <algorithm>

TSL_NS_BEGIN

template<typename T_ACTION, typename T_FINAL>
struct PROCEDURE__ : SchedProcedure {
private:
   OVERRIDE(getAction() -> SchedAction*) {
      return new(cache) T_ACTION;
   }

   OVERRIDE(getFinalAction() -> SchedAction*) {
      return new(cache) T_FINAL;
   }

private:
   enum : size_t {
      alignment = std::max(alignof(T_ACTION), alignof(T_FINAL)),
      size = std::max(sizeof(T_ACTION), sizeof(T_FINAL))
   };

   alignas(alignment) unsigned char cache[size];
};

#define __procedure(...) TSL_NS::PROCEDURE__<__VA_ARGS__>
#define __finally(...) __VA_ARGS__

TSL_NS_END

#endif //TRANS_DSL_2_PROCEDUREHELPER_H
