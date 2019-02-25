cmake_minimum_required(VERSION 3.13)
project(strset)

set(CMAKE_CXX_STANDARD 17)

add_executable(strset example1.c strset.cc strset.h strsetconst.cc strsetconst.h)