cmake_minimum_required(VERSION 3.11)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/**/*.cpp)

set(BANCHO_HOST "127.0.0.1" CACHE STRING "Server bind host")
set(BANCHO_PORT "13000" CACHE STRING "Server bind port")

add_executable(bancho_cpp main.cpp)

target_compile_definitions(bancho_cpp PRIVATE
    BANCHO_HOST="${BANCHO_HOST}"
    BANCHO_PORT=${BANCHO_PORT}
)

target_link_options(bancho_cpp PRIVATE -static)