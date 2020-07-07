cmake_minimum_required(VERSION 3.15)
project(trans_dsl_2)

set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_BUILD_TYPE "Release")

if(UNIX)
#-Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wuseless-cast
#-Wold-style-cast
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wnon-virtual-dtor \
      -Wcast-align -Wunused -Woverloaded-virtual -Wconversion \
      -Wsign-conversion -Wnull-dereference -Wdouble-promotion -Wformat=2 \
      -Wno-invalid-offsetof ")
endif()

include_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party"
        "/usr/local/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/pt"
)

LINK_DIRECTORIES(/usr/local/lib)

add_library(trans-dsl-2 STATIC
        include/trans-dsl/tsl_ns.h
        include/cub/base/Status.h
        include/cub/cub_ns.h
        include/trans-dsl/tsl_status.h
        include/event/event_ns.h include/event/concept/EventId.h include/cub/dci/Role.h
        include/cub/base/Keywords.h include/event/concept/EventInfo.h include/event/Status.h
        include/event/concept/Event.h include/cub/gof/Singleton.h include/cub/base/Unclonable.h
        include/trans-dsl/sched/domain/RuntimeContext.h include/trans-dsl/sched/action/SchedProcedure.h
        src/sched/action/SchedProcedure.cpp include/trans-dsl/utils/ActionStatus.h
        include/trans-dsl/sched/domain/Event.h
        include/trans-dsl/sched/domain/SchedAction.h include/trans-dsl/sched/helper/SequentialHelper.h
        include/trans-dsl/sched/domain/TransactionContext.h include/trans-dsl/sched/action/SchedSequential.h
        src/sched/action/SchedSequential.cpp include/trans-dsl/sched/helper/SyncActionHelper.h
        include/trans-dsl/action/TransactionInfo.h include/trans-dsl/sched/domain/Unknown.h
        include/trans-dsl/sched/domain/InstanceId.h include/trans-dsl/sched/action/SchedSyncAction.h
        include/trans-dsl/sched/helper/AsyncActionHelper.h
        include/trans-dsl/action/SimpleAsyncAction.h
        include/trans-dsl/sched/domain/EventId.h
        include/trans-dsl/action/DummyAsyncAction.h
        include/trans-dsl/action/EventHandlerRegistry.h
        src/action/EventHandlerRegistry.cpp src/event/concept/Event.cpp
        include/trans-dsl/action/P2MFExtractor.h include/event/impl/SimpleEventInfo.h
        include/event/impl/ConsecutiveEventInfo.h
        include/trans-dsl/sched/helper/ProcedureHelper.h
        include/trans-dsl/sched/helper/OptionalHelper.h
        include/trans-dsl/sched/action/SchedOptional.h src/sched/action/SchedOptional.cpp
        include/cub/base/IsClass.h include/trans-dsl/sched/helper/LoopHelper.h
        include/trans-dsl/sched/helper/Pred.h
        include/trans-dsl/sched/helper/LoopPred.h
        include/trans-dsl/sched/helper/LoopPredAction.h include/trans-dsl/sched/action/SchedLoop.h
        src/sched/action/SchedLoop.cpp include/trans-dsl/sched/domain/RuntimeContextAutoSwitch.h
        src/sched/domain/RuntimeContext.cpp include/trans-dsl/sched/helper/ConcurrentHelper.h
        include/trans-dsl/utils/SeqInt.h include/trans-dsl/sched/action/SchedConcurrent.h
        src/sched/action/SchedConcurrent.cpp
        include/trans-dsl/sched/action/SchedTimeGuard.h
        include/trans-dsl/sched/domain/TimerInfo.h include/trans-dsl/porting/timer/TimeLength.h
        include/trans-dsl/porting/timer/TimerId.h include/trans-dsl/sched/domain/RelativeTimer.h
        src/sched/domain/RelativeTimer.cpp include/trans-dsl/porting/timer/PlatformSpecifiedTimer.h
        src/sched/porting/timer/PlatformSpecifiedTimer.cpp src/sched/action/SchedTimeGuard.cpp
        include/trans-dsl/sched/helper/TimeGuardHelper.h
        include/trans-dsl/porting/timer/PlatformSpecifiedTimerInfo.h
        src/sched/porting/timer/PlatformSpecifiedTimerInfo.cpp
        include/trans-dsl/sched/action/ActionPath.h include/trans-dsl/sched/action/SchedSwitchCase.h
        src/sched/action/SchedSwitchCase.cpp include/trans-dsl/sched/helper/SwitchCaseHelper.h
        include/trans-dsl/trans-dsl.h include/trans-dsl/sched/helper/ThrowHelper.h
        include/trans-dsl/sched/action/SchedSafe.h src/sched/action/SchedSafe.cpp
        include/trans-dsl/sched/helper/SafeHelper.h include/trans-dsl/sched/domain/Transaction.h
        include/trans-dsl/sched/domain/RuntimeContextInfo.h src/sched/domain/Transaction.cpp
        include/trans-dsl/sched/helper/FragmentHelper.h
        include/trans-dsl/sched/action/SchedVoid.h src/sched/action/SchedVoid.cpp
        include/trans-dsl/sched/helper/VoidHelper.h include/trans-dsl/sched/action/SchedWait.h
        src/sched/action/SchedWait.cpp include/trans-dsl/sched/helper/WaitHelper.h
        include/trans-dsl/sched/concepts/ActionConcept.h include/trans-dsl/tsl_config.h
        include/trans-dsl/sched/concepts/SchedActionConcept.h include/trans-dsl/sched/concepts/ConceptHelper.h
        include/trans-dsl/sched/concepts/SyncActionClassConcept.h include/trans-dsl/sched/concepts/ClassConcept.h
        include/trans-dsl/sched/helper/MaxSizeCalc.h include/trans-dsl/utils/TypeListExtractor.h
        include/trans-dsl/sched/concepts/PredConcept.h include/cub/utils/RepeatMacros.h
        include/trans-dsl/sched/helper/ActionPathHelper.h include/trans-dsl/sched/helper/VolatileSeq.h
        include/trans-dsl/sched/helper/JoinHelper.h include/trans-dsl/sched/domain/ThreadId.h
        include/trans-dsl/sched/domain/ThreadBitMap.h include/trans-dsl/sched/action/SchedJoin.h
        src/sched/action/SchedJoin.cpp include/trans-dsl/sched/domain/MultiThreadContext.h
        include/trans-dsl/sched/domain/ThreadDoneMsg.h include/trans-dsl/sched/action/SchedFork.h
        src/sched/action/SchedFork.cpp include/trans-dsl/sched/helper/ForkHelper.h
        include/trans-dsl/utils/AssertionHelper.h include/trans-dsl/sched/action/SchedMultiThread.h
        src/sched/action/SchedMultiThread.cpp include/trans-dsl/utils/ThreadActionTrait.h include/cub/base/BitSet.h include/trans-dsl/sched/helper/TransactionHelper.h include/trans-dsl/sched/helper/MultiThreadHelper.h include/trans-dsl/utils/TypeListSpliter.h include/trans-dsl/sched/helper/AutoSeqHelper.h include/trans-dsl/sched/helper/InlineSeqHelper.h include/trans-dsl/sched/helper/AutoActionHelper.h include/trans-dsl/sched/helper/TransactionListenerHelper.h include/trans-dsl/sched/domain/TransactionListener.h include/trans-dsl/sched/domain/ActionId.h include/trans-dsl/sched/domain/ObservedActionIdRegistry.h include/trans-dsl/sched/helper/UserTransListener.h include/trans-dsl/utils/TypeListTransformer.h include/trans-dsl/sched/helper/ActionRealTypeTraits.h include/trans-dsl/sched/domain/TransListenerObservedAids.h)

add_executable(trans-dsl-test
        tests/ut/TestLoop.cpp
        tests/ut/TestOptional.cpp tests/ut/TestProcedure.cpp tests/ut/StupidTransactionContext.h
        tests/ut/TestSequentialAction.cpp)

target_link_libraries(trans-dsl-test trans-dsl-2 cctest_main cctest)

add_executable(trans-dsl-test-catch
        tests/ut/TestSwitchCase.cpp
        tests/ut/TestConcurrent.cpp
        tests/ut/TestSimpleAsyncAction.cpp
        tests/ut/TestTimerGuard.cpp
        tests/ut/TestMultiThread.cpp
        tests/pt/catch.cpp tests/ut/TestTransaction.cpp tests/ut/FragmentTest.cpp tests/ut/ProcedureTest.cpp tests/ut/TransactionListenerTest.cpp)

target_link_libraries(trans-dsl-test-catch trans-dsl-2)

add_executable(trans-dsl-bench
        tests/pt/nanobench.cpp
        tests/pt/dsl-test.cpp
        tests/pt/catch.cpp)

target_link_libraries(trans-dsl-bench trans-dsl-2)
