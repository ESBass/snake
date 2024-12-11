add_subdirectory(SDL)
add_subdirectory(SDL_ttf)

#set(SDL_TTF_LIBRARIES ../bin/lib/SDL3_ttf)

link_directories(${CMAKE_SOURCE_DIR}/lib)
add_library(lib INTERFACE)
target_link_libraries(lib INTERFACE SDL3::SDL3 SDL3_ttf::SDL3_ttf-static)