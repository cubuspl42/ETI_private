cmake_minimum_required(VERSION 2.8)
project(Sudoku_Tomek)

set(SOURCES
    conio2.h
    main.cpp
)

IF(WIN32)
    list(APPEND SOURCES
        conio2_win.h
        conio2_win.cpp
    )
ELSE()
    list(APPEND SOURCES
        conio2_ncurses.h
        conio2_ncurses.cpp
    )
ENDIF(WIN32)

set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
link_directories(${CURSES_LIBRARY_DIRS})

add_executable(sudoku ${SOURCES})
target_link_libraries(sudoku ${CURSES_LIBRARIES})
