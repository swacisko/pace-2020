cmake_minimum_required(VERSION 3.10.2)
project(ExTREEm)

#add_definitions(-std=gnu++0x)
set(CMAKE_CXX_STANDARD 14)

#SET(CMAKE_CXX_FLAGS "-D_GLIBCXX_DEBUG -static -pthread -std=c++14 -O0") # compilation for debugging
#SET(CMAKE_CXX_FLAGS " -static -pthread -std=c++14 -O0") # compilation for profiling

#SET(CMAKE_CXX_FLAGS "-static -pthread -std=c++14 -lm -O2") # compilation for faster execution -O2 uses compiler optimizations, -O0 uses no optimizations at all
#SET(CMAKE_CXX_FLAGS "-static -pthread -std=c++14 -lm -O2 -DNDEBUG") # compilation for faster execution -O2 uses compiler optimizations, -O0 uses no optimizations at all

#remove -pthread
#SET(CMAKE_CXX_FLAGS "-static -std=c++14 -lm -O3") # turbo fast
SET(CMAKE_CXX_FLAGS "-static -std=c++14 -lm -O3 -DNDEBUG") # turbo fast - disabling assertions

#set(SOURCES main.cpp)
include_directories(include include/* include/*/* include/*/*/* include/*/*/*/* )

file(GLOB SOURCES main.cpp #[[./*.cpp]] src/* src/*/* src/*/*/* src/*/*/*/* src/*/*/*/*/* src/*/*/*/*/*/* src/*/*/*/*/*/*/*)
add_executable(ExTREEm ${SOURCES})

#add_subdirectory(algorithms)
#add_subdirectory(algorithms_tests)



#target_link_libraries(AlgorithmsProject_run algorithms)


