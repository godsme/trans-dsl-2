//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_TIMEGUARDHELPER_H
#define TRANS_DSL_2_TIMEGUARDHELPER_H

#include <trans-dsl/sched/action/SchedTimeGuard.h>
#include <trans-dsl/porting/timer/TimerId.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/porting/timer/PlatformSpecifiedTimer.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/helper/AutoActionHelper.h>

TSL_NS_BEGIN

namespace details {

   template<TimerId V_TIMER_ID, typename T_ACTION, Status TIMEOUT_RESULT = Result::TIMEOUT>
   struct TimeGuard final {
      template<TransListenerObservedAids const& AIDs>
      class ActionRealType : public SchedTimeGuard {
         using Action = ActionRealTypeTraits_t<AIDs, T_ACTION>;
         CONCEPT_ASSERT(SchedActionConcept<Action>);

         IMPL_ROLE_WITH_VAR(SchedAction, action);
         IMPL_ROLE_WITH_VAR(RelativeTimer, timer);

         Action action;
         PlatformSpecifiedTimer timer{V_TIMER_ID};
         OVERRIDE(getTimeoutResult() const -> Status) { return TIMEOUT_RESULT; }
      public:
         using ThreadActionCreator = ThreadCreator_t<Action>;
      };
   };

   template<TimerId V_TIMER_ID, typename T_ACTION, Status TIMEOUT_RESULT = Result::TIMEOUT>
   using TimeGuard_t = typename TimeGuard<V_TIMER_ID, T_ACTION, TIMEOUT_RESULT>::template ActionRealType<EmptyAids>;
}

TSL_NS_END

#define __time_guard(timerId, ...) \
TSL_NS::details::TimeGuard<timerId, TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>>

#define __def_time_guard(timerId, ...) \
TSL_NS::details::TimeGuard_t<timerId, TSL_NS::details::AutoAction::SequentialTrait_t<__VA_ARGS__>>

#endif //TRANS_DSL_2_TIMEGUARDHELPER_H
