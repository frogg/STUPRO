# include QT
find_package(Qt4 REQUIRED QtTest)
include(${QT_USE_FILE})

# test sources
file(GLOB Test_SOURCES ./**/*.cpp)
file(GLOB Test_HEADERS ./**/*.hpp)

# include main library headers
include_directories(../src/util/)

# go through every test source file and create a seperate executable for it
# in order to allow ctest to find the tests
foreach(testSourceFile ${Test_SOURCES})
  # get the header file from the source file by changing its extension
  string(REPLACE ".cpp" ".hpp" testHeaderFile ${testSourceFile})
  # create a MOC file from the header
  qt4_wrap_cpp(testMocFile ${testHeaderFile})

  # extract the name of the test from the name of the source file
  get_filename_component(testName ${testSourceFile} NAME)
  string(REPLACE ".cpp" ".test" testName ${testName})

  # register the executable
  add_executable(${testName} ${testSourceFile} ${testMocFile})

  # link the executable
  target_link_libraries(${testName} ${QT_LIBRARIES} libKronos)
endforeach(testSourceFile ${Test_SOURCES})
