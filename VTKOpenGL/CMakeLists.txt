cmake_minimum_required(VERSION 2.8)
project(exe)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
file(GLOB SOURCES
    Main.cpp
)
add_executable(exe ${SOURCES})
target_link_libraries(exe ${VTK_LIBRARIES})


