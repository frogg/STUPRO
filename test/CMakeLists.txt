cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

# test runner class
set(MAIN_CLASS
    ${CMAKE_CURRENT_SOURCE_DIR}/TestRunner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TestRunner.hpp
)

set(TEST_MOC_SRCS
    "${CMAKE_CURRENT_SOURCE_DIR}/TestRunner.hpp"
)

# set(CMAKE_AUTOMOC TRUE)

# find ParaView headers and libraries
find_package(ParaView REQUIRED)
include(${PARAVIEW_USE_FILE})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# find Qt headers and libraries
if("${KRONOS_QT_VERSION}" STREQUAL "5")
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Gui REQUIRED)
    find_package(Qt5Network REQUIRED)
    find_package(Qt5Widgets REQUIRED)
else()
    find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
    include(${QT_USE_FILE})
endif()

# wrap classes inheriting from QObject
if("${KRONOS_QT_VERSION}" STREQUAL "5")
    QT5_WRAP_CPP(TEST_MOC_FILES ${TEST_MOC_SRCS})
else()
    QT4_WRAP_CPP(TEST_MOC_FILES ${TEST_MOC_SRCS})
endif()

# find GTest headers and libraries
find_package(GoogleTest REQUIRED)

# test sources
file(GLOB_RECURSE Test_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/**/Test*.cpp)

# include main library headers
include_directories(${CMAKE_SOURCE_DIR}/src)

# include external library headers
include_directories(${CMAKE_SOURCE_DIR}/libs)

# include GTest headers
include_directories(${GTEST_INCLUDE_DIRS})

# create an executable for the test runner
add_executable(UnitTester ${MAIN_CLASS} ${Test_SOURCES} ${TEST_MOC_FILES})

# set C++ standard to C++11
set_property(TARGET UnitTester PROPERTY CXX_STANDARD 11)

# link against kronos and GTest libraries
target_link_libraries(UnitTester
  KronosPlugin
  ${GTEST_BOTH_LIBRARIES}
)

# link against Qt libraries
if("${KRONOS_QT_VERSION}" STREQUAL "5")
    target_link_libraries(UnitTester
        Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets
    )
else()
    target_link_libraries(UnitTester
        ${QT_LIBRARIES}
    )
endif()

# change binary output paths
set_target_properties(UnitTester
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${TEST_BIN_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${TEST_BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${TEST_BIN_DIR}"
)

# make sure not to copy or delete the resource folder if we are doing an in-source build
if(NOT ("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${TEST_BIN_DIR}"))
    # copy over test resource folder after each build
    add_custom_command(
        TARGET UnitTester
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${TEST_BIN_DIR}/res
    )
    # copy over the test server folder after each vuild
    add_custom_command(
        TARGET UnitTester
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/test-server ${TEST_BIN_DIR}/test-server
    )
    # delete the resource folder when executing make clean
    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${TEST_BIN_DIR}/res")
endif()
