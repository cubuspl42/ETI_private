cmake_minimum_required(VERSION 3.1)

set(PROJECT a)

project(${PROJECT})

# include(FindPkgConfig)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/out")

set(SOURCES
        src/main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

# pkg_search_module(SDL2 REQUIRED sdl2)
# pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
# pkg_search_module(SDL2NET REQUIRED SDL2_net>=2.0.0)

# include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2NET_INCLUDE_DIRS})
# link_directories(${SDL2_LIBRARY_DIRS} ${SDL2IMAGE_LIBRARY_DIRS} ${SDL2NET_LIBRARY_DIRS})

add_executable(${PROJECT} ${SOURCES})

target_link_libraries(${PROJECT} Threads::Threads)
# target_link_libraries(${PROJECT} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2NET_LIBRARIES})

add_custom_command(TARGET ${PROJECT} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT}>)
