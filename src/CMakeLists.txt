
# Sources
file(GLOB Library_SOURCES ./*/**/*.cpp)
file(GLOB Library_HEADERS ./*/**/*.hpp)

add_executable(kronos main.cpp)

# Build a library from all sources
add_library(kronosLib ${Library_SOURCES} ${Library_HEADERS})

# link the library to the main executable
target_link_libraries(kronos kronosLib)
