cmake_minimum_required(VERSION 2.8)
project(Arkanoid)
include(FindPkgConfig)

set(SOURCES
    main.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS} ${SDL2IMAGE_LIBRARY_DIRS})

add_executable(arkanoid ${SOURCES})
target_link_libraries(arkanoid ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})
add_custom_command(TARGET arkanoid PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:arkanoid>)
