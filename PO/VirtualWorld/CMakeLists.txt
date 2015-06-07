cmake_minimum_required(VERSION 2.8)
project(VirtualWorld)
set(EXECUTABLE_NAME virtualworld)

set(SOURCES
    main.cpp
    Organism.h
    Organism.cpp
    Animal.h
    Animal.cpp
    Human.h
    Human.cpp
    VariousAnimals.h
    VariousAnimals.cpp
    Plant.h
    Plant.cpp
    VariousPlants.h
    VariousPlants.cpp
    World.h
    World.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
link_directories(${CURSES_LIBRARY_DIRS})

add_executable(${EXECUTABLE_NAME} ${SOURCES})
target_link_libraries(${EXECUTABLE_NAME} ${CURSES_LIBRARIES})
