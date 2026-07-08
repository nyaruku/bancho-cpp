cmake_minimum_required(VERSION 3.11)
project(bancho_cpp LANGUAGES CXX)

set(BASE_INCLUDE_PATH "${CMAKE_SOURCE_DIR}/includes")
set(BASE_LIB_PATH "${CMAKE_SOURCE_DIR}/libs")

include_directories(${BASE_INCLUDE_PATH})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(src)