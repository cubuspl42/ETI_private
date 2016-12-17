cmake_minimum_required(VERSION 2.8)
set(PROJECT p2)
project(${PROJECT})

set(SOURCES
        main.cpp
        PagedFile.h
        PagedFile.cpp
        BPage.h
        BPage.cpp
        IndexedFile.h
        IndexedFile.cpp
        Record.cpp
        Record.h
        BTree.cpp
        BTree.h
        common.h
        ContentFile.cpp
        ContentFile.h
        BStorage.h
        BStorage.cpp
        )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic -Werror")

add_executable(${PROJECT} ${SOURCES})
