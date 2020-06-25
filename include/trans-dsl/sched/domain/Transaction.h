//
// Created by Darwin Yuan on 2020/6/15.
//

#ifndef TRANS_DSL_2_TRANSACTION_H
#define TRANS_DSL_2_TRANSACTION_H

#include <cub/dci/Role.h>
#include <trans-dsl/tsl_status.h>
#include <trans-dsl/sched/domain/Event.h>

TSL_NS_BEGIN

DEFINE_ROLE(Transaction) {
  ABSTRACT(start() -> Status);
  ABSTRACT(handleEvent(Event const&) -> Status);
  ABSTRACT(stop(Status = Result::FORCE_STOPPED) -> Status);
  ABSTRACT(kill(Status = Result::FORCE_STOPPED) -> void);

  auto startWithEvent(Event const&) -> Status;
};

TSL_NS_END

#endif //TRANS_DSL_2_TRANSACTION_H
