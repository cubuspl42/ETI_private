cmake_minimum_required(VERSION 2.8)
set(PROJECT p2)
project(${PROJECT})

set(SOURCES
        BNode.h
        BNode.cpp
        IndexedFile.h
        IndexedFile.cpp
        Record.cpp
        Record.h
        BTree.cpp
        BTree.h
        common.h
        ContentFile.cpp
        ContentFile.h
        BTreeStorage.h
        BTreeStorage.cpp
        BTreeHeader.h
        MemStorage.cpp
        MemStorage.h
        FileStorage.cpp
        FileStorage.h
        BElement.h
        )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -pedantic -Werror")

add_executable(${PROJECT} ${SOURCES} main.cpp)
add_executable(${PROJECT}_test ${SOURCES} test.cpp)