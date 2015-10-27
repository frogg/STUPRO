cmake_minimum_required(VERSION 2.8)
project(kronos)

add_subdirectory(src)
add_subdirectory(test)

enable_testing()

add_test(NAME quadTreeSatelliteImage COMMAND Test)


# find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
# include(${QT_USE_FILE})
#
# file(GLOB SOURCES
#   src/**/*.hpp
#   src/**/*.cpp
#   src/**/*.h
#   src/**/*.c
# )
#
# add_executable(kronos ${SOURCES})
# target_link_libraries(kronos ${VTK_LIBRARIES})
# target_link_libraries(kronos ${QT_LIBRARIES})
