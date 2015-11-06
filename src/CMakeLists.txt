# main entry point of the application
set(MAIN_CLASS ./Main.cpp)

set(CMAKE_AUTOMOC TRUE)

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

find_package(Threads)

# find VTK
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

# copy resources
file(COPY  ${CMAKE_SOURCE_DIR}/glsl DESTINATION ${CMAKE_BINARY_DIR})
file(COPY  ${CMAKE_SOURCE_DIR}/tiles DESTINATION ${CMAKE_BINARY_DIR})

# sources
file(GLOB Library_SOURCES ./*.cpp ./**/*.cpp)
file(GLOB Library_HEADERS ./*.hpp ./**/*.hpp ../src/*.hpp ../src/**/*.hpp)

# exclude the main class from the library files
list(REMOVE_ITEM Library_SOURCES MAIN_CLASS)
list(REMOVE_ITEM Library_HEADERS MAIN_CLASS)

# build a library from all sources
add_library(libKronos ${Library_SOURCES} ${Library_HEADERS})

# register the main executable
add_executable(kronos ${MAIN_CLASS})

# set C++ standard to C++11
set_property(TARGET libKronos PROPERTY CXX_STANDARD 11)
set_property(TARGET kronos PROPERTY CXX_STANDARD 11)

target_link_libraries(libKronos ${CMAKE_THREAD_LIBS_INIT})

# link against Qt libraries
if(${KRONOS_QT_VERSION} EQUAL 4)
    target_link_libraries(libKronos ${QT_LIBRARIES})
    target_link_libraries(kronos ${QT_LIBRARIES})
elseif(${KRONOS_QT_VERSION} EQUAL 5)
    target_link_libraries(libKronos Qt5::Core Qt5::Gui Qt5::Network)
    target_link_libraries(kronos Qt5::Widgets)
endif()

# link VTK
target_link_libraries(libKronos ${VTK_LIBRARIES})

# link the library to the main executable
target_link_libraries(kronos libKronos)

# copy resource directory to the build directory
file(REMOVE_RECURSE ./res)
file(COPY ../res DESTINATION ./)
