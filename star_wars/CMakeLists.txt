cmake_minimum_required(VERSION 3.13)
project(StarWars)

set(CMAKE_CXX_STANDARD 17)

add_executable(star_wars starwars_example.cc helper.cc helper.h imperialfleet.cc imperialfleet.h rebelfleet.cc rebelfleet.h battle.cc battle.h)