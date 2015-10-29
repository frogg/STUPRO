cmake_minimum_required(VERSION 2.8)
project(kronos)

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
