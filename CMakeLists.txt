cmake_minimum_required(VERSION 3.15)

project(Snake)

add_executable(${PROJECT_NAME} src/main.c)

add_subdirectory(lib/sdl)

target_include_directories(${PROJECT_NAME} PUBLIC lib/sdl/include)

target_link_directories(${PROJECT_NAME} PRIVATE lib/sdl/src)

target_link_libraries(${PROJECT_NAME} sdl3)