cmake_minimum_required(VERSION 3.11)

include(FetchContent)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/**/*.cpp)

add_executable(bancho_cpp main.cpp)