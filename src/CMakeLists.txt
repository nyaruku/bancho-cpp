cmake_minimum_required(VERSION 3.11)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/**/*.cpp)

if(NOT DEFINED BANCHO_HOST OR BANCHO_HOST STREQUAL "")
    set(BANCHO_HOST "127.0.0.1")
endif()
if(NOT DEFINED BANCHO_PORT OR BANCHO_PORT STREQUAL "")
    set(BANCHO_PORT "13380")
endif()
if(NOT DEFINED BANCHO_THREADS OR BANCHO_THREADS STREQUAL "")
    set(BANCHO_THREADS "2")
endif()

add_executable(bancho_cpp main.cpp)

target_compile_definitions(bancho_cpp PRIVATE
    BANCHO_HOST="${BANCHO_HOST}"
    BANCHO_PORT=${BANCHO_PORT}
    BANCHO_THREADS=${BANCHO_THREADS}
)

target_link_options(bancho_cpp PRIVATE -static)