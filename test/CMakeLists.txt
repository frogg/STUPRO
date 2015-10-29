# include QT
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Network)
find_package(Qt5Widgets)

# test sources
file(GLOB Test_SOURCES ./**/*.cpp)
file(GLOB Test_HEADERS ./**/*.hpp)

# include main library headers
include_directories(../src/util/)
include_directories(../include/)

# go through every test source file and create a seperate executable for it
# in order to allow ctest to find the tests
foreach(testSourceFile ${Test_SOURCES})
  # get the header file from the source file by changing its extension
  string(REPLACE ".cpp" ".hpp" testHeaderFile ${testSourceFile})

  # extract the name of the test from the name of the source file
  get_filename_component(testName ${testSourceFile} NAME)
  string(REPLACE ".cpp" ".test" testName ${testName})

  # register the executable
  add_executable(${testName} ${testSourceFile} ${testHeaderFile})

  # link the executable
  target_link_libraries(${testName} libKronos Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets)
endforeach(testSourceFile ${Test_SOURCES})
