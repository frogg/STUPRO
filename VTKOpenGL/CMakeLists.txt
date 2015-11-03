cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
project(exe)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

INCLUDE_DIRECTORIES(../Libraries)

file(COPY  ${CMAKE_SOURCE_DIR}/Shader DESTINATION ${CMAKE_BINARY_DIR})
file(COPY  ${CMAKE_SOURCE_DIR}/Resources DESTINATION ${CMAKE_BINARY_DIR})
file(GLOB SOURCES
    *.cpp *.hpp
)
add_executable(exe ${SOURCES})
target_link_libraries(exe ${VTK_LIBRARIES})
