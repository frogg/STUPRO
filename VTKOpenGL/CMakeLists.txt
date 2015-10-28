cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
project(globe)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
file(COPY  ${CMAKE_SOURCE_DIR}/Shader DESTINATION ${CMAKE_BINARY_DIR})
file(COPY  ${CMAKE_SOURCE_DIR}/Resources DESTINATION ${CMAKE_BINARY_DIR})
file(GLOB SOURCES
    Main.cpp
    VTKOpenGL.cpp
    StuproInteractor.cpp
    Utils.cpp
    GlobeTile.cpp
    Globe.cpp
)
add_executable(globe ${SOURCES})
target_link_libraries(globe ${VTK_LIBRARIES})
