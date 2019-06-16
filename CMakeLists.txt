cmake_minimum_required(VERSION 3.10.2)
project(nekomimi-gameboy-emulator)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
find_package(SDL2 REQUIRED)

aux_source_directory(./src DIR_SRCS)
add_executable(run-emulator ${DIR_SRCS})
target_link_libraries(run-emulator SDL2::Main)