cmake_minimum_required(VERSION 3.10.2)
project(nekomimi-gameboy-emulator)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

aux_source_directory(./src DIR_SRCS)
add_executable(run-emulator ${DIR_SRCS})

target_link_libraries(run-emulator ${SDL2_LIBRARIES})