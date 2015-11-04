cmake_minimum_required(VERSION 2.8.6)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
project(kronos)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
file(COPY  ${CMAKE_SOURCE_DIR}/glsl DESTINATION ${CMAKE_BINARY_DIR})
file(COPY  ${CMAKE_SOURCE_DIR}/tiles DESTINATION ${CMAKE_BINARY_DIR})

file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS src/*.hpp)

set(KRONOS_QT_VERSION 4)

add_subdirectory(src)
add_subdirectory(test)

# enable ctest capabilities
enable_testing()

# tests are added here and not in the ./test folder to make it possible to
# simply run `make test` in the root directory to run the tests
file(GLOB testClasses ./test/**/Test*.cpp)
foreach(test ${testClasses})
  get_filename_component(testName ${test} NAME_WE)
  add_test(NAME ${testName} COMMAND "test/UnitTester" ${testName})
endforeach(test ${testClasses})

add_executable(kronos ${SOURCES} ${HEADERS})
target_link_libraries(kronos ${VTK_LIBRARIES})
