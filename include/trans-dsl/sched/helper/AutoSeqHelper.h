//
// Created by Darwin Yuan on 2020/7/5.
//

#ifndef TRANS_DSL_2_AUTOSEQHELPER_H
#define TRANS_DSL_2_AUTOSEQHELPER_H

#include <trans-dsl/sched/action/SchedProcedure.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/helper/SequentialHelper.h>
#include <trans-dsl/utils/TypeListSpliter.h>

TSL_NS_BEGIN

namespace details {
   template<typename SIGNATURE>
   struct AutoSeq final {
      template<CONCEPT(SchedActionConcept) ... T_ACTIONS>
      struct Inner : SIGNATURE {
         using type = __sequential(T_ACTIONS...);
      };

      template<CONCEPT(SchedActionConcept) HEAD>
      struct Inner<HEAD> : SIGNATURE {
         using type = HEAD;
      };
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_AUTOSEQHELPER_H
