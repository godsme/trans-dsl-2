//
// Created by Darwin Yuan on 2021/4/26.
//

#ifndef TRANS_DSL_2_IGNOREEVENTWHENHELPER_H
#define TRANS_DSL_2_IGNOREEVENTWHENHELPER_H

#include <trans-dsl/sched/action/SchedIgnoreEventWhen.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/helper/AutoActionHelper.h>
#include <trans-dsl/sched/helper/ActionRealTypeTraits.h>
#include <type_traits>

TSL_NS_BEGIN

namespace details {
    template<typename EV_MATCHER, typename T_ACTION>
    struct IgnoreEventWhen {
        template<TransListenerObservedAids const& AIDs>
        class ActionRealType : public SchedIgnoreEventWhen {
            using Action = ActionRealTypeTraits_t<AIDs, T_ACTION>;
            CONCEPT_ASSERT(SchedActionConcept<Action>);

            OVERRIDE(matches(EV_NS::EventId id) const -> bool) {
                return EV_MATCHER::matches(id);
            }
            IMPL_ROLE_WITH_VAR(SchedAction, action);
            Action action;
        public:
            using ThreadActionCreator = ThreadCreator_t<Action>;
        };
    };

    template<EV_NS::EventId ... EV_IDs>
    struct EventMatcher {
        static_assert(sizeof...(EV_IDs) > 0);
        static auto matches(EV_NS::EventId id) -> bool {
            return ((id == EV_IDs) || ...);
        }
    };

    template<EV_NS::EventId  EV_ID>
    auto DeduceEventMatcher() -> EventMatcher<EV_ID>;

    template<typename EV_MATCHER, typename = std::void_t<decltype(EV_MATCHER::matches(std::declval<EV_NS::EventId>()))>>
    auto DeduceEventMatcher() -> EV_MATCHER;

    template<typename EV_MATCHER, typename ... T_ACTIONS>
    using IgnoreEventWhen_t = IgnoreEventWhen<EV_MATCHER, TSL_NS::details::AutoAction::SequentialTrait_t<T_ACTIONS...>>;

    template<typename EV_MATCHER, typename ... T_ACTIONS>
    using DefIgnoreEventWhen_t = typename IgnoreEventWhen_t<EV_MATCHER, T_ACTIONS...>::template ActionRealType<EmptyAids>;
}

TSL_NS_END

#define __events(...) TSL_NS::details::EventMatcher<__VA_ARGS__>
#define __ignore_event_when(ev, ...) \
TSL_NS::details::IgnoreEventWhen_t<decltype(TSL_NS::details::DeduceEventMatcher<ev>()), __VA_ARGS__>                                     \

#define __def_ignore_event_when(ev, ...) \
   TSL_NS::details::DefIgnoreEventWhen_t<decltype(TSL_NS::details::DeduceEventMatcher<ev>()), __VA_ARGS__>

#endif //TRANS_DSL_2_IGNOREEVENTWHENHELPER_H
