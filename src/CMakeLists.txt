cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

# find pthread (C++11 threading) library and headers
find_package(Threads REQUIRED)

# find ParaView headers and libraries
find_package(ParaView 4.3.1 REQUIRED)
include(${PARAVIEW_USE_FILE})
include(ParaViewBranding)

# find Qt headers and libraries
find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
include(${QT_USE_FILE})

# automatically wrap *.ui files and classes inheriting from QObject
set(CMAKE_AUTOMOC TRUE)
QT4_WRAP_UI(UISrcs StuproWindow.ui)
# fallback (works only for CMake 3.0 and above)
# set(CMAKE_AUTOUIC TRUE)

# source files responsible for creating the ParaView window
set(ParaView_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/StuproWindow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/StuproWindow.hpp
	${UISrcs}
)

# all other Kronos source files
file(GLOB_RECURSE StuproView_SOURCE_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/**/*.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/**/*.cxx"
	"${CMAKE_CURRENT_SOURCE_DIR}/**/*.h"
)

# Custom Files needed to be included in add_paraview_plugin
file(GLOB CUSTOM_VIEW
        "${CMAKE_CURRENT_SOURCE_DIR}/View/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/View/*.cxx"
)
file(GLOB CUSTOM_FILTER
        "${CMAKE_CURRENT_SOURCE_DIR}/Filter/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Filter/*.cxx"
)
file(GLOB CUSTOM_SOURCE
        "${CMAKE_CURRENT_SOURCE_DIR}/Sources/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Sources/*.cxx"
)
file(GLOB CUSTOM_READER
        "${CMAKE_CURRENT_SOURCE_DIR}/Reader/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Reader/*.cxx"
)

include_directories(${CMAKE_SOURCE_DIR}/libs)

# directories to include (important for auto-generated code)
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/View/
	${CMAKE_CURRENT_SOURCE_DIR}/Filter/
	${CMAKE_CURRENT_SOURCE_DIR}/Sources/
	${CMAKE_CURRENT_SOURCE_DIR}/Reader/
)

# load the glsl-shaders to use in code:
# these can be used using the filename (without extension) as
# const char* variables
encode_files_as_strings(StuproView_ENCODED_SHADERS
	${CMAKE_CURRENT_SOURCE_DIR}/../res/glsl/GlobeShader_fsh.glsl
	${CMAKE_CURRENT_SOURCE_DIR}/../res/glsl/GlobeShader_vsh.glsl
)

# register a ParaView plugin
add_paraview_plugin(
	KronosPlugin
	"1.0.0"
	REQUIRED_ON_SERVER
	REQUIRED_ON_CLIENT

    SERVER_MANAGER_SOURCES
        ${CUSTOM_VIEW}
        ${CUSTOM_FILTER}
        ${CUSTOM_SOURCE}
        ${CUSTOM_READER}
	SERVER_MANAGER_XML
		View/KronosView.xml
		Filter/KronosFilter.xml
		Sources/KronosSources.xml
		Reader/KronosReader.xml
	SOURCES
		${StuproView_SOURCE_FILES}
		${StuproView_ENCODED_SHADERS}
)

# link against Qt libraries
target_link_libraries(KronosPlugin PRIVATE ${QT_LIBRARIES})

# use C++11 to compile the plugin
set_property(TARGET KronosPlugin PROPERTY CXX_STANDARD 11)

# register a ParaView client
build_paraview_client(
	paraview_stupro
	TITLE "ParaView - Kronos-Edition"
	ORGANIZATION "Kitware Inc."
	SPLASH_IMAGE "Splash.png"
	VERSION_MAJOR 1
	VERSION_MINOR 0
	VERSION_PATCH 0
	PVMAIN_WINDOW StuproWindow
	PVMAIN_WINDOW_INCLUDE StuproWindow.hpp
	REQUIRED_PLUGINS KronosPlugin
	SOURCES ${ParaView_SOURCE_FILES}
	GUI_CONFIGURATION_XMLS
      ${CMAKE_CURRENT_SOURCE_DIR}/ParaViewSources.xml
	${CMAKE_CURRENT_SOURCE_DIR}/ParaViewFilters.xml
)

# use C++11 to compile the client
set_property(TARGET paraview_stupro PROPERTY CXX_STANDARD 11)

# copy over all required resources
file(
	COPY ${CMAKE_CURRENT_SOURCE_DIR}/../res
	DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
	PATTERN glsl EXCLUDE
)
