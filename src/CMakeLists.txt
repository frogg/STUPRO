# main entry point of the application
set(MAIN_CLASS ./main.cpp)

# include QT
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Network)
find_package(Qt5Widgets)

# sources
file(GLOB Library_SOURCES ./*.cpp ./**/*.cpp)
file(GLOB Library_HEADERS ./*.hpp ./**/*.hpp ../include/*.hpp ../include/**/*.hpp)

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

# link qt to the kronos lib and the main application
target_link_libraries(libKronos Qt5::Core Qt5::Gui Qt5::Network)
target_link_libraries(kronos Qt5::Widgets)

# link the library to the main executable
target_link_libraries(kronos libKronos)
