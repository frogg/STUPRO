cmake_minimum_required(VERSION 2.8)
project(exe)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
include(${QT_USE_FILE})

file(GLOB SOURCES
    toCompile/*.h
    toCompile/*.cpp
)
add_executable(exe ${SOURCES})
target_link_libraries(exe ${VTK_LIBRARIES})
target_link_libraries(exe ${QT_LIBRARIES})
target_link_libraries(exe curl)

set_property(TARGET exe PROPERTY CXX_STANDARD 14)
