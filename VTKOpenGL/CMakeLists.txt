cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
project(exe)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
file(COPY  ${CMAKE_SOURCE_DIR}/Shader DESTINATION ${CMAKE_BINARY_DIR})
file(GLOB SOURCES
    Main.cpp
)
add_executable(exe ${SOURCES})
target_link_libraries(exe ${VTK_LIBRARIES})
