cmake_minimum_required(VERSION 2.8)
project(Sudoku)
#include(FindPkgConfig)

set(SOURCES
    config.h
    common.h
    algorithm.h
    list.h
    map.h
    xml.h
    xml.cpp 
    sudoku.h
    sudoku.cpp
    ncurses_gui.h
    ncurses_gui.cpp
    main.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
link_directories(${CURSES_LIBRARY_DIRS})

add_executable(sudoku ${SOURCES})
target_link_libraries(sudoku ${CURSES_LIBRARIES})
add_custom_command(TARGET sudoku PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:sudoku>)
