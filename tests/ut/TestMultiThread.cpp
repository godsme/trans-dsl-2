//
// Created by Darwin Yuan on 2020/7/2.
//

#include <catch.hpp>
#include <trans-dsl/trans-dsl.h>
#include "StupidTransactionContext.h"
#include "SimpleActionsDefs.h"
#include <iostream>

namespace {

   using namespace TSL_NS;

   SCENARIO("__multi_thread with successful sync action") {
      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      WHEN("exec with a sync action, should return SUCCESS") {
         __multi_thread(__sync(SyncAction1)) action;
         REQUIRE(Result::SUCCESS == action.exec(context));

         AND_THEN("exec it again, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == action.exec(context));
         }

         AND_WHEN("event 1 received, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
         }
         AND_WHEN("stop it, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == action.stop(context, Result::DUPTID));
         }
      }
   }

   SCENARIO("SchedMultiThread with failed sync action") {
      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      WHEN("exec with a sync action, should return FAILED") {
         __multi_thread(__sync(FailedSyncAction4)) action;

         REQUIRE(Result::FAILED == action.exec(context));

         AND_THEN("exec it again, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == action.exec(context));
         }

         AND_WHEN("event 1 received, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
         }
         AND_WHEN("stop it, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == action.stop(context, Result::DUPTID));
         }
      }
   }

   SCENARIO("SchedMultiThread with successful async action") {
      StupidTransactionContext context{};

      __multi_thread(__asyn(AsyncAction1)) action;

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{ 10  };
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      WHEN("event 1 received, should return FATAL_BUG") {
         REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
      }

      WHEN("stop it, should return FATAL_BUG") {
         REQUIRE(Result::FATAL_BUG == action.stop(context, Result::TIMEOUT));
      }

      WHEN("exec with a sync action, should return CONTINUE") {
         REQUIRE(Result::CONTINUE == action.exec(context));
         AND_THEN("exec it again, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == action.exec(context));
         }

         AND_THEN("event 1 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
         }
         AND_THEN("stop it, should return FORCE_STOPPED") {
            REQUIRE(Result::FORCE_STOPPED == action.stop(context, Result::DUPTID));
         }
         AND_THEN("event 2 received, should return UNKNOWN_EVENT") {
            REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event2));
            AND_WHEN("event 1 received, should return SUCCESS") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
            }
         }
         AND_GIVEN("a killed scheduler") {
            action.kill(context, Result::DUPTID);
            THEN("stop it, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == action.stop(context, Result::DUPTID));
            }
         }
      }
   }

   SCENARIO("SchedMultiThread with failed async action") {
      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg3 msg3{ 10  };
      const EV_NS::ConsecutiveEventInfo event3{EV_MSG_3, msg3};

      WHEN("exec, should return CONTINUE") {
         __multi_thread(__asyn(FailedAsyncAction3)) action;

         REQUIRE(Result::CONTINUE == action.exec(context));
         AND_THEN("exec it again, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == action.exec(context));
         }

         AND_THEN("event 3 received, should return FAILED") {
            REQUIRE(Result::FAILED == action.handleEvent(context, event3));
         }

         AND_THEN("event 1 received, should return UNKNOWN_EVENT") {
            REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event1));
            AND_WHEN("event 3 received, should return FAILED") {
               REQUIRE(Result::FAILED == action.handleEvent(context, event3));
            }
         }

         AND_THEN("stop it, should return FORCE_STOPPED") {
            REQUIRE(Result::FORCE_STOPPED == action.stop(context, Result::DUPTID));
         }

         AND_GIVEN("a killed scheduler") {
            action.kill(context, Result::DUPTID);
            AND_THEN("exec it again, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == action.exec(context));
            }
            THEN("event 3 received, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event3));
            }
            THEN("stop it, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == action.stop(context, Result::DUPTID));
            }
         }
      }
   }

   SCENARIO("SchedMultiThread with fork action") {
      StupidTransactionContext context{};

      const Msg1 msg1{ 10, 20 };
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{ 10 };
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      const Msg4 msg4{ 10 };
      const EV_NS::ConsecutiveEventInfo event4{EV_MSG_4, msg4};

      WHEN("exec with a fork action") {
         __multi_thread(
            __sequential(
               __fork(1, __asyn(AsyncAction1)),
               __fork(2, __asyn(AsyncAction4)),
               __asyn(AsyncAction2))) action;

         REQUIRE(Result::CONTINUE == action.exec(context));

         //std::cout << sizeof(MainAction) << std::endl;

         WHEN("exec again, should return FATAL_BUG") {
            REQUIRE(Result::FATAL_BUG == action.exec(context));
         }

         WHEN("stop, should return FORCE_STOPPED") {
            REQUIRE(Result::FORCE_STOPPED == action.stop(context, Result::DUPTID));
            WHEN("event 1 received, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
            }
            WHEN("event 2 received, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event2));
            }
            WHEN("event 4 received, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event4));
            }
         }

         WHEN("event 2 received, should return CONTINUE") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
            WHEN("event 1 received, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
               THEN("event 4 received, should return FATAL_BUG") {
                  REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event4));
               }
            }
         }

         WHEN("event 1 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            THEN("event 4 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.handleEvent(context, event4));
               THEN("event 2 received, should return CONTINUE") {
                  REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
               }
            }
            THEN("event 2 received, should return CONTINUE") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
            }
         }
         WHEN("event 4 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event4));
            THEN("event 1 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
               THEN("event 2 received, should return CONTINUE") {
                  REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
               }
            }
            THEN("event 2 received, should return CONTINUE") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
            }
         }
      }
   }

   SCENARIO("SchedMultiThread with fork & join_all") {
      StupidTransactionContext context{};

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{10};
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      const Msg4 msg4{10};
      const EV_NS::ConsecutiveEventInfo event4{EV_MSG_4, msg4};

      WHEN("exec with a fork action") {
         using MainAction =
         __sequential(
            __fork(1, __asyn(AsyncAction1)),
            __fork(2, __asyn(AsyncAction4)),
            __asyn(AsyncAction2),
            __join());

         __multi_thread(MainAction) action;

         REQUIRE(Result::CONTINUE == action.exec(context));

         WHEN("stop, should return FORCE_STOPPED") {
            REQUIRE(Result::FORCE_STOPPED == action.stop(context, Result::DUPTID));
            THEN("event 4 received, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event4));
            }
            THEN("stop again, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == action.stop(context, Result::DUPTID));
            }
         }

         WHEN("killed") {
            action.kill(context, Result::DUPTID);
            THEN("event 4 received, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event4));
            }
            THEN("stop, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == action.stop(context, Result::DUPTID));
            }
         }

         WHEN("event 2 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));
            THEN("event 4 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.handleEvent(context, event4));
               THEN("event 1 received, should return SUCCESS") {
                  REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
               }
            }
         }

         WHEN("event 1 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            THEN("event 2 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));
               THEN("event 4 received, should return SUCCESS") {
                  REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
               }
            }
            AND_WHEN("stop, should return FORCE_STOPPED") {
               REQUIRE(Result::FORCE_STOPPED == action.stop(context, Result::DUPTID));
               THEN("event 4 received, should return FATAL_BUG") {
                  REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event4));
               }
            }
         }

         WHEN("event 4 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event4));
            THEN("event 2 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));
               THEN("event 1 received, should return SUCCESS") {
                  REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
               }
            }
         }

         WHEN("event 4 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event4));
            THEN("event 1 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
               THEN("event 2 received, should return SUCCESS") {
                  REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
               }
            }
         }

         WHEN("event 1 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            THEN("event 4 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.handleEvent(context, event4));
               THEN("event 2 received, should return SUCCESS") {
                  REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
               }
            }
         }
      }
   }

   SCENARIO("SchedMultiThread with late fork") {
      StupidTransactionContext context{};

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{10};
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      const Msg4 msg4{10};
      const EV_NS::ConsecutiveEventInfo event4{EV_MSG_4, msg4};

      WHEN("exec with a fork action") {
         using MainAction =
         __sequential(
            __fork(1, __sequential(__asyn(AsyncAction1), __fork(2, __asyn(AsyncAction4)))),
            __asyn(AsyncAction2),
            __join(2));

         __multi_thread(MainAction) action;

         REQUIRE(Result::CONTINUE == action.exec(context));

         WHEN("event 4 received, should return UNKNOWN_EVENT") {
            REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event4));
            THEN("event 2 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));
               THEN("event 4 received, should return UNKNOWN_EVENT") {
                  REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event4));
                  THEN("event 1 received, should return CONTINUE") {
                     REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
                     THEN("event 4 received, should return SUCCESS") {
                        REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
                     }
                  }
               }
            }
         }
      }
   }

   SCENARIO("SchedMultiThread with join a non-exist thread") {
      StupidTransactionContext context{};

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{10};
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      const Msg4 msg4{10};
      const EV_NS::ConsecutiveEventInfo event4{EV_MSG_4, msg4};

      WHEN("exec with a fork action") {
         using MainAction =
         __sequential(
            __fork(1, __sequential(__asyn(AsyncAction1), __fork(2, __asyn(AsyncAction4)))),
            __asyn(AsyncAction2),
            __join(2));

         __multi_thread(MainAction) action;

         REQUIRE(Result::CONTINUE == action.exec(context));

         WHEN("event 4 received, should return UNKNOWN_EVENT") {
            REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event4));
            THEN("event 2 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));
               THEN("event 4 received, should return UNKNOWN_EVENT") {
                  REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event4));
                  THEN("event 1 received, should return CONTINUE") {
                     REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
                     THEN("event 4 received, should return SUCCESS") {
                        REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
                     }
                  }
               }
            }
         }
      }
   }

   SCENARIO("SchedMultiThread with a fork in fork") {
      StupidTransactionContext context{};

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{10};
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      const Msg4 msg4{10};
      const EV_NS::ConsecutiveEventInfo event4{EV_MSG_4, msg4};

      WHEN("exec with a fork action") {
         using MainAction =
         __sequential(
            __fork(1, __sequential(__asyn(AsyncAction1), __fork(2, __asyn(AsyncAction4)))),
            __asyn(AsyncAction2),
            __join());

         __multi_thread(MainAction) action;

         REQUIRE(Result::CONTINUE == action.exec(context));

         WHEN("event 4 received, should return UNKNOWN_EVENT") {
            REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event4));
            THEN("event 2 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));
               THEN("event 4 received, should return UNKNOWN_EVENT") {
                  REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event4));
                  THEN("event 1 received, should return CONTINUE") {
                     REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
                     THEN("event 4 received, should return SUCCESS") {
                        REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
                     }
                  }
               }
            }
         }

         WHEN("event 1 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            THEN("event 2 received, should return CONTINUE") {
               REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));
               THEN("event 4 received, should return SUCCESS") {
                  REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
               }
            }
            AND_WHEN("stop, should return FORCE_STOPPED") {
               REQUIRE(Result::FORCE_STOPPED == action.stop(context, Result::DUPTID));
               THEN("event 4 received, should return FATAL_BUG") {
                  REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event4));
               }
            }
         }
      }
   }

   SCENARIO("SchedMultiThread with a immediate fork in fork") {
      StupidTransactionContext context{};

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{10};
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      const Msg4 msg4{10};
      const EV_NS::ConsecutiveEventInfo event4{EV_MSG_4, msg4};

      WHEN("exec with a fork action") {
         using MainAction =
         __sequential(
            __fork(1, __fork(2, __asyn(AsyncAction4))),
            __asyn(AsyncAction2),
            __join());

         __multi_thread(MainAction) action;

         REQUIRE(Result::CONTINUE == action.exec(context));

         THEN("event 2 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));
            THEN("event 4 received, should return SUCCESS") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event4));
            }
         }

         THEN("event 4 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event4));
            THEN("event 2 received, should return SUCCESS") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
            }
         }
      }
   }

   SCENARIO("SchedMultiThread with a successful procedure") {
      StupidTransactionContext context{};

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{10};
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      const Msg4 msg4{10};
      const EV_NS::ConsecutiveEventInfo event4{EV_MSG_4, msg4};

      WHEN("exec with a fork action") {
         using MainAction =
         __sequential(
            __fork(1, __asyn(AsyncAction1)),
            __procedure
              ( __asyn(AsyncAction2)
              , __finally(__on_status(Result::FORCE_STOPPED, __asyn(AsyncAction4)))),
            __join());

         __multi_thread(MainAction) action;

         REQUIRE(Result::CONTINUE == action.exec(context));

         THEN("event 1 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            THEN("event 2 received, should return SUCCESS") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
            }
         }

         THEN("event 2 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));
            THEN("event 1 received, should return SUCCESS") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
            }
         }

         WHEN("stop, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.stop(context, Result::DUPTID));
            THEN("event 2 received, should return UNKNOWN_EVENT") {
               REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event2));
               THEN("event 1 received, should return UNKNOWN_EVENT") {
                  REQUIRE(Result::UNKNOWN_EVENT == action.handleEvent(context, event1));
                  THEN("event 4 received, should return SUCCESS") {
                     REQUIRE(Result::FORCE_STOPPED == action.handleEvent(context, event4));
                  }
               }
            }
         }

         THEN("event 2 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));
            WHEN("stop, should return FORCE_STOPPED") {
               REQUIRE(Result::FORCE_STOPPED == action.stop(context, Result::DUPTID));
            }
         }
      }
   }

   SCENARIO("SchedMultiThread with a failed join") {
      StupidTransactionContext context{};

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{10};
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      WHEN("exec with a fork action with wrong joined thread ids") {
         using MainAction =
         __sequential(
            __concurrent(__fork(1, __asyn(AsyncAction1)), __fork(2, __asyn(AsyncAction2))),
            __join(2, 3, 4, 5));

         __multi_thread(MainAction) action;

         REQUIRE(Result::CONTINUE == action.exec(context));

         THEN("if event 2 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
         }

         THEN("if event 1 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event1));
            AND_THEN("if event 2 received, should return SUCCESS") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event2));
            }
         }
      }
   }

   SCENARIO("SchedMultiThread with a concurrent fork") {
      StupidTransactionContext context{};

      const Msg1 msg1{10, 20};
      const EV_NS::ConsecutiveEventInfo event1{EV_MSG_1, msg1};

      const Msg2 msg2{10};
      const EV_NS::ConsecutiveEventInfo event2{EV_MSG_2, msg2};

      WHEN("exec with a fork action") {
         using MainAction =
         __sequential(
            __concurrent(__fork(1, __asyn(AsyncAction1)), __fork(2, __asyn(AsyncAction2))),
            __join(1));

         __multi_thread(MainAction) action;

         REQUIRE(Result::CONTINUE == action.exec(context));

         THEN("if event 1 received, should return SUCCESS") {
            REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
         }

         THEN("stop it, should return FORCE_STOPPED") {
            REQUIRE(Result::FORCE_STOPPED == action.stop(context, Result::TIMEOUT));
            AND_THEN("if event 1 received, should return FATAL_BUG") {
               REQUIRE(Result::FATAL_BUG == action.handleEvent(context, event1));
            }
         }

         THEN("if event 2 received, should return CONTINUE") {
            REQUIRE(Result::CONTINUE == action.handleEvent(context, event2));
            AND_THEN("event 1 received, should return SUCCESS") {
               REQUIRE(Result::SUCCESS == action.handleEvent(context, event1));
            }
         }
      }
   }
}
