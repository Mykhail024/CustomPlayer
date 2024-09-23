cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_NAME CustomPlayer)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

project(${PROJECT_NAME})

include_directories(src/ui)

add_subdirectory(src/ui)
