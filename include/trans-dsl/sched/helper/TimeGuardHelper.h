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

   template<TimerId V_TIMER_ID, Status TIMEOUT_RESULT, typename ... ACTIONS>
   auto deduceTimeGuard() -> TSL_NS::details::TimeGuard<V_TIMER_ID, TSL_NS::details::AutoAction::SequentialTrait_t<ACTIONS...>, TIMEOUT_RESULT>;

   template<TimerId V_TIMER_ID, typename ... ACTIONS>
   auto deduceTimeGuard() -> TSL_NS::details::TimeGuard<V_TIMER_ID, TSL_NS::details::AutoAction::SequentialTrait_t<ACTIONS...>>;
}

TSL_NS_END

#define __time_guard(timerId, ...) \
decltype(TSL_NS::details::deduceTimeGuard<timerId, __VA_ARGS__>())

#define __def_time_guard(timerId, ...) \
__time_guard(timerId, __VA_ARGS__)::template ActionRealType<EmptyAids>

#endif //TRANS_DSL_2_TIMEGUARDHELPER_H
