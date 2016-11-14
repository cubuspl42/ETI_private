cmake_minimum_required(VERSION 2.8)
set(PROJECT p1)
project(${PROJECT})

set(SOURCES
        main.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic -Werror")


# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/out")

add_executable(${PROJECT} ${SOURCES})
