cmake_minimum_required(VERSION 3.13)
project(tst)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_COMPILER  "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Og -g -O2 -std=c++17")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

add_executable(tst tst.h example.cc)