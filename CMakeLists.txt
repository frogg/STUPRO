cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
project(kronos)

set(KRONOS_QT_VERSION CACHE STRING "4")

# register subprojects
add_subdirectory(src)
