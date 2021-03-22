//
// Created by Darwin Yuan on 2020/6/13.
//

#ifndef TRANS_DSL_2_CONCURRENTHELPER_H
#define TRANS_DSL_2_CONCURRENTHELPER_H

#include <trans-dsl/tsl_config.h>
#include <trans-dsl/tsl_ns.h>
#include <trans-dsl/utils/SeqInt.h>
#include <trans-dsl/sched/helper/ActionTuple.h>
#include <trans-dsl/sched/helper/MultiAction.h>
#include <trans-dsl/sched/action/SchedConcurrent.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/domain/TransListenerObservedAids.h>

TSL_NS_BEGIN

struct SchedAction;

namespace details {

   template<typename ... T_ACTIONS>
   struct Concurrent final  : MultiAction<SchedConcurrent, T_ACTIONS...>{
      static constexpr size_t Num_Of_Actions = sizeof...(T_ACTIONS);
      static_assert(Num_Of_Actions >= 2, "# of concurrent actions should be at least 2");
   };

   template<typename ... Ts>
   using Concurrent_t = typename Concurrent<Ts...>::template ActionRealType<EmptyAids>;
}

#define __concurrent(...) TSL_NS::details::Concurrent<__VA_ARGS__>
#define __def_concurrent(...) TSL_NS::details::Concurrent_t<__VA_ARGS__>

TSL_NS_END

#endif //TRANS_DSL_2_CONCURRENTHELPER_H
