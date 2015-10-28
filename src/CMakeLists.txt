# main entry point of the application
set(MAIN_CLASS ./main.cpp)

# include QT
find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
include(${QT_USE_FILE})

# sources
file(GLOB Library_SOURCES ./**/*.cpp)
file(GLOB Library_HEADERS ./**/*.hpp ../include/**/*.hpp)

# exclude the main class from the library files
list(REMOVE_ITEM Library_SOURCES MAIN_CLASS)
list(REMOVE_ITEM Library_HEADERS MAIN_CLASS)

# build a library from all sources
add_library(libKronos ${Library_SOURCES} ${Library_HEADERS})

# register the main executable
add_executable(kronos ${MAIN_CLASS})

# link qt to the kronos lib and the main application
target_link_libraries(libKronos ${QT_LIBRARIES})
target_link_libraries(kronos ${QT_LIBRARIES})

# link the library to the main executable
target_link_libraries(kronos libKronos)
