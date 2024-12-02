cmake_minimum_required(VERSION 3.15)

project(Snake)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(lib)

add_subdirectory(src)