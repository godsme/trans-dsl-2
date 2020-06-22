//
// Created by Darwin Yuan on 2020/6/22.
//

#ifndef TRANS_DSL_2_SYNCACTIONCLASSCONCEPT_H
#define TRANS_DSL_2_SYNCACTIONCLASSCONCEPT_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/domain/Event.h>
#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/concepts/ClassConcept.h>

TSL_NS_BEGIN

   struct TransactionInfo;

#if __CONCEPT_ENABLED
template<typename T>
concept SyncActionClassConcept = ClassConcept<T> &&
   requires( T t, TransactionInfo& trans) {
      { t(trans) }  -> std::same_as<TSL_NS::Status>;
   };
#else

struct SyncActionTypeClass {
   template<typename T>
   __DEF_TYPE_CLASS(T action, const TransactionInfo& trans) __AS(
      __METHOD(TSL_NS::Status, action(trans))
   );
};

template<typename T>
constexpr bool SyncActionClassConcept = ClassConcept<T> && IsTypeClass<SyncActionTypeClass, T>;

#endif

TSL_NS_END

#endif //TRANS_DSL_2_SYNCACTIONCLASSCONCEPT_H
