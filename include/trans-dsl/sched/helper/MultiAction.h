//
// Created by Darwin Yuan on 2021/3/22.
//

#ifndef TRANS_DSL_2_MULTIACTION_H
#define TRANS_DSL_2_MULTIACTION_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/sched/helper/ActionTuple.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/domain/TransListenerObservedAids.h>

TSL_NS_BEGIN

struct SchedAction;

namespace details {
    template<typename ACTION_TYPE, typename ... T_ACTIONS>
    class MultiAction  {
        static constexpr size_t Num_Of_Actions = sizeof...(T_ACTIONS);

        template<typename ... Tss>
        struct Base  {
            using ThreadActionCreator = ThreadCreator_t<Tss...>;
        protected:
            ActionTuple<Tss...> actions_;
        };

        template<TransListenerObservedAids const& AIDs>
        struct Traits {
            template<typename T>
            using ToActionRealType = ActionRealTypeTraits<AIDs, T>;

            using Base = CUB_NS::Transform_t<ToActionRealType, Base, T_ACTIONS...>;
        };

    public:
        template<TransListenerObservedAids const& AIDs>
        class ActionRealType : public ACTION_TYPE, Traits<AIDs>::Base {
            using Base = typename Traits<AIDs>::Base;
        public:
            using ThreadActionCreator = typename Traits<AIDs>::Base::ThreadActionCreator;

        private:
            OVERRIDE(getNumOfActions() const -> SeqInt) {
                return Num_Of_Actions;
            }

            OVERRIDE(get(SeqInt seq) -> SchedAction*) {
                return Base::actions_.get(seq);
            }
        };
    };
}

TSL_NS_END

#endif //TRANS_DSL_2_MULTIACTION_H
