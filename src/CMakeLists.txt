add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE main.c)

target_include_directories(${PROJECT_NAME} PRIVATE)

target_link_libraries(${PROJECT_NAME} PRIVATE lib)