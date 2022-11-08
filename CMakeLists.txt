cmake_minimum_required(VERSION 2.8)

project(testing)

enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -O3 -msse4.2 -DNDEBUG=1")

add_subdirectory(benchs)
add_subdirectory(test)
