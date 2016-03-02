cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
project(kronos)

set(KRONOS_QT_VERSION "4" CACHE STRING "Qt Version to use when building Kronos")
# set Paraview's Qt version to the same used for Kronos
set(PARAVIEW_QT_VERSION ${KRONOS_QT_VERSION})

set(KRONOS_BIN_DIR ${CMAKE_BINARY_DIR}/src CACHE PATH "Output path of Kronos binaries")
set(TEST_BIN_DIR ${CMAKE_BINARY_DIR}/test CACHE PATH "Output path of Unit Test binaries")

# use ccache if possible to speed up the build process
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

# change the output folder for libraries and binaries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${KRONOS_BIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${KRONOS_BIN_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${KRONOS_BIN_DIR})

# register subprojects
add_subdirectory(src)

# add test subproject only on unix (workaround until GTest is introduced)
if(UNIX)
  add_subdirectory(test)
  # enable ctest capabilities
  enable_testing()

  # tests are added here and not in the ./test folder to make it possible to
  # simply run `make test` in the root directory to run the tests
  file(GLOB_RECURSE testClasses ./test/**/Test*.cpp)
  foreach(test ${testClasses})
    get_filename_component(testName ${test} NAME_WE)
    add_test(
      NAME ${testName}
      COMMAND UnitTester --gtest_filter=${testName}.*
      WORKING_DIRECTORY ${TEST_BIN_DIR}
    )
  endforeach()
endif()
