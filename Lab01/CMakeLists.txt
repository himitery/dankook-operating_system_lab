cmake_minimum_required(VERSION 3.21)
project(Lab01)

set(CMAKE_CXX_STANDARD 14)

add_executable(Lab01 include/lab1_sched_types.h lab1_sched.cpp lab1_sched_test.cpp)
