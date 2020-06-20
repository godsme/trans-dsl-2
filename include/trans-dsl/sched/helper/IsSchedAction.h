//
// Created by Darwin Yuan on 2020/6/13.
//

#ifndef TRANS_DSL_2_ISSCHEDACTION_H
#define TRANS_DSL_2_ISSCHEDACTION_H

#include <type_traits>
#include <trans-dsl/sched/domain/SchedAction.h>

TSL_NS_BEGIN

namespace details {
   template<typename T1, typename T2>
   using IsBothSchedAction =
      std::enable_if_t<
         std::is_base_of_v<TSL_NS::SchedAction, T1> &&
         std::is_base_of_v<TSL_NS::SchedAction, T2>>;
}

TSL_NS_END

#endif //TRANS_DSL_2_ISSCHEDACTION_H
