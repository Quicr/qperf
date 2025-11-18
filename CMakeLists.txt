# SPDX-FileCopyrightText: Copyright (c) 2025 Cisco Systems
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.10)

project(qperf)

#=============================================================================#
# Add dependencies
#=============================================================================#

set(CPM_USE_LOCAL_PACKAGES ON)

include(cmake/CPM.cmake)

## hack fix for picotls and other older submodules of libquicr
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

CPMAddPackage("gh:Quicr/libquicr#main")
CPMAddPackage("gh:jarro2783/cxxopts@3.3.1")

#=============================================================================#
# Build QPerf executable
#=============================================================================#

add_executable(qperf_meeting src/qperf_meeting.cpp src/publisher_track_handler.cpp src/subscriber_track_handler.cpp)
target_link_libraries(qperf_meeting PRIVATE quicr cxxopts spdlog::spdlog)
target_include_directories(qperf_meeting PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(qperf_meeting PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
    $<$<CXX_COMPILER_ID:MSVC>: >
)

set_target_properties(qperf_meeting PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS OFF
)

target_compile_definitions(qperf_meeting PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)

#=============================================================================#
# Build QPerf Publication executable
#=============================================================================#

add_executable(qperf_pub src/qperf_pub.cpp src/publisher_track_handler.cpp)
target_link_libraries(qperf_pub PRIVATE quicr cxxopts spdlog::spdlog)
target_include_directories(qperf_pub PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(qperf_pub PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
    $<$<CXX_COMPILER_ID:MSVC>: >
)

set_target_properties(qperf_pub PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS OFF
)

target_compile_definitions(qperf_pub PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)

#=============================================================================#
# Build QPerf Subscription executable
#=============================================================================#

add_executable(qperf_sub src/qperf_sub.cpp src/subscriber_track_handler.cpp)
target_link_libraries(qperf_sub PRIVATE quicr cxxopts spdlog::spdlog)
target_include_directories(qperf_sub PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(qperf_sub PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
    $<$<CXX_COMPILER_ID:MSVC>: >
)

set_target_properties(qperf_sub PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS OFF
)

target_compile_definitions(qperf_sub PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
