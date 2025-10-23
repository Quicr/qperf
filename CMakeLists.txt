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

add_executable(qperf src/qperf.cpp src/qperf_pub.cpp src/qperf_sub.cpp)
target_link_libraries(qperf PRIVATE quicr cxxopts spdlog::spdlog)
target_include_directories(qperf PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(qperf PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
    $<$<CXX_COMPILER_ID:MSVC>: >
)

set_target_properties(qperf PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS OFF
)

target_compile_definitions(qperf PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
