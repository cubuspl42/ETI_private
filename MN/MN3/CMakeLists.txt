cmake_minimum_required(VERSION 2.8)
project(MN3)

set(SOURCES
        JAKUB_TRZEBIATOWSKI_PR_3_GRUPA_6.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/out")

add_executable(mn3 ${SOURCES})
