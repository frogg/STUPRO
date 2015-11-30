# test runner class
set(MAIN_CLASS
    ${CMAKE_CURRENT_SOURCE_DIR}/TestRunner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TestRunner.hpp
)

set(CMAKE_AUTOMOC TRUE)
set(KRONOS_QT_VERSION 4)

# find ParaView headers and libraries
find_package(ParaView REQUIRED)
include(${PARAVIEW_USE_FILE})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# find Qt headers and libraries
if(${KRONOS_QT_VERSION} EQUAL 4)
    find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
    include(${QT_USE_FILE})
elseif(${KRONOS_QT_VERSION} EQUAL 5)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Gui REQUIRED)
    find_package(Qt5Network REQUIRED)
    find_package(Qt5Widgets REQUIRED)
endif()

# find GTest headers and libraries
find_package(GTest REQUIRED)

# test sources
file(GLOB_RECURSE Test_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/**/Test*.cpp)

# include main library headers
include_directories(${CMAKE_SOURCE_DIR}/src)

# include external library headers
include_directories(${CMAKE_SOURCE_DIR}/libs)

# include CppUnit headers
include_directories(${GTEST_INCLUDE_DIR})

# create an executable for the test runner
add_executable(UnitTester ${MAIN_CLASS} ${Test_SOURCES})

# set C++ standard to C++11
set_property(TARGET UnitTester PROPERTY CXX_STANDARD 11)

# link against kronos and CppUnit libraries
target_link_libraries(UnitTester
  StuproViewPlugin
  ${GTEST_BOTH_LIBRARIES}
)

# link against Qt libraries
if(${KRONOS_QT_VERSION} EQUAL 4)
    target_link_libraries(UnitTester
        ${QT_LIBRARIES}
    )
elseif(${KRONOS_QT_VERSION} EQUAL 5)
    target_link_libraries(UnitTester
        Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets
    )
endif()

# copy over test resource folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
