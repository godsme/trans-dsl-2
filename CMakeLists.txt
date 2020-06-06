cmake_minimum_required(VERSION 3.16)
project(trans_dsl_2)

set(CMAKE_CXX_STANDARD 17)

#if(UNIX)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-invalid-offsetof -std=c++17")
#endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

#add_subdirectory("src")

add_executable(trans_dsl_2 main.cpp include/trans-dsl/tsl_ns.h include/cub/base/Status.h include/cub/cub_ns.h include/trans-dsl/tsl_status.h include/event/event_ns.h include/event/concept/EventId.h include/cub/dci/Role.h include/cub/base/Keywords.h include/event/concept/EventInfo.h include/event/Status.h include/event/concept/Event.h)