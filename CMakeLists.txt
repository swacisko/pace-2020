cmake_minimum_required(VERSION 3.20)
project(ExTREEm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(CMAKE_CXX_FLAGS "-std=c++23 -lm -O3") # turbo fast

#set(SOURCES main.cpp)
include_directories(include
        include/combinatorics
        include/datastructures
        include/frameworks
        include/graphs
        include/graphs
        include/graphs/*
        include/graphs/*/*
        include/graphs/*/*/*
        include/numbertheory
        include/utils

        include/contests/PACE20
        include/contests/PACE20/depthtreecreators
        include/contests/PACE20/separatorcreators
        include/contests/PACE20/separatorminimizers
        include/CONTESTS/experiments
)

file(GLOB SOURCES
        src/combinatorics/CombinatoricUtils.cpp
        src/combinatorics/sequences/combinations/Combinations.cpp
        src/datastructures/*.cpp
        src/frameworks/*.cpp
        src/graphs/*.cpp
        src/graphs/*/*.cpp
        src/graphs/*/*/*.cpp
        src/numbertheory/*.cpp
        src/utils/*.cpp

        src/contests/PACE20/*.cpp
        src/contests/PACE20/depthtreecreators/*.cpp
        src/contests/PACE20/separatorcreators/*.cpp
        src/contests/PACE20/separatorminimizers/*.cpp
        src/CONTESTS/experiments/*.cpp
)

#add_executable(ExTREEm ${SOURCES}  main.cpp )


add_executable(exp1 ${SOURCES}
        include/contests/experiments/Exp1.h
        src/CONTESTS/experiments/Exp1.cpp
)
