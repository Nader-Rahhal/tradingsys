cmake_minimum_required(VERSION 3.20)
project(orderbook CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g")

add_executable(orderbook src/main.cpp)
target_include_directories(orderbook PRIVATE include)

add_executable(perf_bench tests/bench.cpp)
target_include_directories(perf_bench PRIVATE include)