//
// Created by Darwin Yuan on 2020/7/5.
//

#ifndef TRANS_DSL_2_AUTOSEQHELPER_H
#define TRANS_DSL_2_AUTOSEQHELPER_H

#include <trans-dsl/sched/action/SchedProcedure.h>
#include <trans-dsl/sched/concepts/SchedActionConcept.h>
#include <trans-dsl/utils/ThreadActionTrait.h>
#include <trans-dsl/sched/helper/SequentialHelper.h>

TSL_NS_BEGIN

namespace details {
   struct DummySignature {};

   template<typename SIGNATURE = DummySignature>
   struct AutoSeq final {
      template<typename ... T_ACTIONS>
      struct Inner : SIGNATURE {
         using type = __sequential(T_ACTIONS...);
      };

      template<typename HEAD>
      struct Inner<HEAD> : SIGNATURE {
         using type = HEAD;
      };
   };
}

TSL_NS_END

#endif //TRANS_DSL_2_AUTOSEQHELPER_H
