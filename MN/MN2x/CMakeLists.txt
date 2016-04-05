cmake_minimum_required(VERSION 2.8)
project(MN2)

set(SOURCES
    main.cpp
        gauss.cpp gauss.h Matrix.cpp Matrix.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/out")

add_executable(mn2 ${SOURCES})
