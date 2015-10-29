# test runner class
set(MAIN_CLASS ./TestRunner.cpp)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# include QT
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(CppUnit REQUIRED)

# test sources
file(GLOB Test_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ./**/Test*.cpp)
file(GLOB Test_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ./**/Test*.hpp)

# include main library headers
include_directories(../src/util/)
include_directories(../include/)

# include CppUnit headers
include_directories(${CPPUNIT_INCLUDE_DIR})

# create an executable for the test runner
add_executable(UnitTester ${MAIN_CLASS} ${Test_SOURCES} ${Test_HEADERS})
target_link_libraries(UnitTester
  libKronos
  Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets
  cppunit
)
