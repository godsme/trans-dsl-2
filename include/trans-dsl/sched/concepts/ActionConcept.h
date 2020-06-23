//
// Created by Darwin Yuan on 2020/6/16.
//

#ifndef TRANS_DSL_2_ACTIONCONCEPT_H
#define TRANS_DSL_2_ACTIONCONCEPT_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/sched/domain/Event.h>
#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/concepts/ConceptHelper.h>

#if __has_include(<concepts>)
#include <concepts>
#endif

TSL_NS_BEGIN

struct TransactionInfo;

#if __CONCEPT_ENABLED
template<typename T>
concept ActionConcept = requires
        ( T t,
          TransactionInfo& trans,
          const Event& event,
          Status cause) {
    { t.exec(trans) }               -> std::same_as<Status>;
    { t.handleEvent(trans, event) } -> std::same_as<Status>;
    { t.kill(trans, cause) }        -> std::same_as<Status>;
};
#else

struct ActionTypeClass {
   template<typename T>
   __DEF_TYPE_CLASS(T action, const TransactionInfo& trans, const Event& event, Status cause) {
      __METHOD(Status, action.exec(trans));
      __METHOD(Status, action.handleEvent(trans, event));
      __METHOD(Status, action.kill(trans, cause));
   }
};

template<typename T>
constexpr bool ActionConcept = IsTypeClass<ActionTypeClass, T>;

#endif

TSL_NS_END

#endif //TRANS_DSL_2_ACTIONCONCEPT_H
