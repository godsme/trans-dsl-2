//
// Created by Darwin Yuan on 2020/6/28.
//

#ifndef TRANS_DSL_2_PREDCONCEPT_H
#define TRANS_DSL_2_PREDCONCEPT_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/domain/Event.h>
#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/concepts/ClassConcept.h>

TSL_NS_BEGIN

   struct TransactionInfo;

#if __CONCEPT_ENABLED

template<typename T>
concept PredConcept = ClassConcept<T> &&
   requires( T t, TransactionInfo& trans) {
      { t(trans) }  -> std::same_as<bool>;
   };

#else

struct PredTypeClass {
   template<typename T>
   __DEF_TYPE_CLASS(T pred, TransactionInfo const& trans) {
      __METHOD(bool, pred(trans));
   }
};

template<typename T>
constexpr bool PredConcept = ClassConcept<T> && IsTypeClass<PredTypeClass, T>;

#endif

TSL_NS_END

#endif //TRANS_DSL_2_PREDCONCEPT_H
