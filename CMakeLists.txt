cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
project(kronos)

set(KRONOS_QT_VERSION 4)

# register subprojects
add_subdirectory(src)
IF(NOT WIN32)
	add_subdirectory(test)
ENDIF()

# enable ctest capabilities
enable_testing()

# tests are added here and not in the ./test folder to make it possible to
# simply run `make test` in the root directory to run the tests
file(GLOB_RECURSE testClasses ./test/**/Test*.cpp)
foreach(test ${testClasses})
  get_filename_component(testName ${test} NAME_WE)
  add_test(
    NAME ${testName}
    COMMAND UnitTester ${testName}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test
  )
endforeach()
