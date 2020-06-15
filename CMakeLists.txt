cmake_minimum_required(VERSION 3.16)
project(trans_dsl_2)

set(CMAKE_CXX_STANDARD 17)

find_package(PythonInterp)

#if(UNIX)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -std=c++17")
#endif()

include_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "/usr/local/include"
)

#add_subdirectory("src")

LINK_DIRECTORIES(/usr/local/lib)

add_executable(trans-dsl-test include/trans-dsl/tsl_ns.h
        include/cub/base/Status.h
        include/cub/cub_ns.h
        include/trans-dsl/tsl_status.h
        include/event/event_ns.h include/event/concept/EventId.h include/cub/dci/Role.h include/cub/base/Keywords.h include/event/concept/EventInfo.h include/event/Status.h include/event/concept/Event.h include/cub/gof/Singleton.h include/cub/base/Unclonable.h include/trans-dsl/sched/concept/RuntimeContext.h include/trans-dsl/sched/action/SchedProcedure.h src/sched/action/SchedProcedure.cpp include/trans-dsl/utils/ActionStatus.h include/trans-dsl/sched/concept/FinalAction.h include/trans-dsl/sched/concept/Event.h include/trans-dsl/sched/concept/SchedAction.h include/trans-dsl/sched/helper/SequentialHelper.h include/trans-dsl/sched/concept/TransactionContext.h include/trans-dsl/sched/action/SchedSequential.h src/sched/action/SchedSequential.cpp include/trans-dsl/sched/helper/SyncActionHelper.h include/trans-dsl/action/TransactionInfo.h include/trans-dsl/sched/concept/Unknown.h include/trans-dsl/sched/concept/InstanceId.h include/trans-dsl/sched/action/SchedSyncAction.h include/trans-dsl/sched/helper/AsyncActionHelper.h include/trans-dsl/action/SimpleAsyncAction.h include/trans-dsl/sched/concept/EventId.h include/trans-dsl/action/DummyAsyncAction.h include/trans-dsl/action/EventHandlerRegistry.h src/action/EventHandlerRegistry.cpp src/event/concept/Event.cpp include/trans-dsl/action/P2MFExtractor.h include/event/impl/SimpleEventInfo.h include/event/impl/ConsecutiveEventInfo.h tests/ut/TestSImpleAsyncAction.cpp tests/ut/StupidTransactionContext.h tests/ut/TestSequentialAction.cpp tests/ut/SimpleActionsDefs.h include/trans-dsl/sched/helper/ProcedureHelper.h tests/ut/TestProcedure.cpp include/trans-dsl/sched/helper/OptionalHelper.h include/trans-dsl/sched/action/SchedOptional.h src/sched/action/SchedOptional.cpp tests/ut/TestOptional.cpp include/cub/base/IsClass.h include/trans-dsl/sched/helper/LoopHelper.h include/trans-dsl/sched/helper/Pred.h tests/ut/TestLoop.cpp include/trans-dsl/sched/helper/LoopPred.h include/trans-dsl/sched/helper/LoopPredAction.h include/trans-dsl/sched/action/SchedLoop.h src/sched/action/SchedLoop.cpp include/trans-dsl/sched/concept/RuntimeContextAutoSwitch.h src/sched/concept/RuntimeContext.cpp include/trans-dsl/sched/helper/ConcurrentHelper.h include/trans-dsl/utils/SeqInt.h include/trans-dsl/sched/action/SchedConcurrent.h src/sched/action/SchedConcurrent.cpp tests/ut/TestConcurrent.cpp include/trans-dsl/sched/helper/IsSchedAction.h include/trans-dsl/sched/action/SchedTimerGuard.h include/trans-dsl/sched/concept/TimerInfo.h include/trans-dsl/porting/timer/TimeLength.h include/trans-dsl/porting/timer/TimerId.h include/trans-dsl/sched/concept/RelativeTimer.h src/sched/concept/RelativeTimer.cpp include/trans-dsl/porting/timer/PlatformSpecifiedTimer.h src/sched/porting/timer/PlatformSpecificTimer.cpp src/sched/action/SchedTimerGuard.cpp include/trans-dsl/sched/helper/TimerGuardHelper.h tests/ut/TestTimerGuard.cpp include/trans-dsl/porting/timer/PlatformSpecifiedTimerInfo.h src/sched/porting/timer/PlatformSpecifiedTimerInfo.cpp)

target_link_libraries(trans-dsl-test cctest_main cctest)