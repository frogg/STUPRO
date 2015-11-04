cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
project(globe)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
file(COPY  ${CMAKE_SOURCE_DIR}/glsl DESTINATION ${CMAKE_BINARY_DIR})
file(COPY  ${CMAKE_SOURCE_DIR}/tiles DESTINATION ${CMAKE_BINARY_DIR})

file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS src/*.hpp)

add_executable(globe ${SOURCES} ${HEADERS})
target_link_libraries(globe ${VTK_LIBRARIES})
