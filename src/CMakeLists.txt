# on windows, we'll need CMake >= 3.4.0 to be able to export all symbols of the plugin
if (WIN32)
	cmake_minimum_required(VERSION 3.4.0 FATAL_ERROR)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
else()
	cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
endif()

# files to be moced for the plugin part
set(PLUGIN_MOC_SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/View/Widgets/CityListModel.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/View/Widgets/PlaceSearchDockWidget.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/View/Widgets/PlaceSearchWidget.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Kronos.h"
)

# files to be moced for the paraview part
set(PARAVIEW_MOC_SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/StuproWindow.hpp"
)

set(PRINT_CLANG_COMPLETE NO CACHE BOOL "Generate output to be pasted into a .clang_complete file")

# find pthread (C++11 threading) library and headers
find_package(Threads REQUIRED)

# find ParaView headers and libraries
find_package(ParaView 4.3.1 REQUIRED)
include(${PARAVIEW_USE_FILE})
include(ParaViewBranding)

# find Qt headers and libraries
if("${KRONOS_QT_VERSION}" STREQUAL "5")
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Gui REQUIRED)
    find_package(Qt5Network REQUIRED)
    find_package(Qt5Widgets REQUIRED)
else()
	find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)
	include(${QT_USE_FILE})
endif()

# wrap classes inheriting from QObject
if("${KRONOS_QT_VERSION}" STREQUAL "5")
	QT5_WRAP_CPP(PLUGIN_MOC_FILES ${PLUGIN_MOC_SRCS})
	QT5_WRAP_CPP(PARAVIEW_MOC_FILES ${PARAVIEW_MOC_SRCS})
	QT5_WRAP_UI(UISrcs StuproWindow.ui)
else()
	QT4_WRAP_CPP(PLUGIN_MOC_FILES ${PLUGIN_MOC_SRCS})
	QT4_WRAP_CPP(PARAVIEW_MOC_FILES ${PARAVIEW_MOC_SRCS})
	QT4_WRAP_UI(UISrcs StuproWindow.ui)
endif()

# fallback (works only for CMake 3.0 and above)
# set(CMAKE_AUTOUIC TRUE)

# source files responsible for creating the ParaView window
set(ParaView_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/StuproWindow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/StuproWindow.hpp
	${PARAVIEW_MOC_SRCS}
	${UISrcs}
)

# all other Kronos source files
file(GLOB_RECURSE StuproView_SOURCE_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/Kronos.*"
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
	${CMAKE_CURRENT_SOURCE_DIR}/View/Widgets/
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

# register methods to be called on startup and shutdown of the plugin
add_paraview_auto_start(Kronos_Main_Interfaces Kronos_Main_Interfaces_SRCS
	CLASS_NAME Kronos
	STARTUP onStartup
	SHUTDOWN onShutdown
)

# register the place search widget
add_paraview_dock_window(
	PLACE_SEARCH_WIDGET_OUTIFACES
	PLACE_SEARCH_WIDGET_OUTSRCS
	CLASS_NAME PlaceSearchDockWidget
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
	GUI_INTERFACES
		${PLACE_SEARCH_WIDGET_OUTIFACES}
		${Kronos_Main_Interfaces}
	GUI_SOURCES
		${PLACE_SEARCH_WIDGET_OUTSRCS}
	SOURCES
		${StuproView_SOURCE_FILES}
		${StuproView_ENCODED_SHADERS}
		${PLACE_SEARCH_WIDGET_OUTSRCS}
		${PLUGIN_MOC_FILES}
		${Kronos_Main_Interfaces_SRCS}
)

# link against Qt libraries
if("${KRONOS_QT_VERSION}" STREQUAL "5")
	target_link_libraries(KronosPlugin PRIVATE
		Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets
	)
else()
	target_link_libraries(KronosPlugin PRIVATE ${QT_LIBRARIES})
endif()

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
	SOURCES
		${ParaView_SOURCE_FILES}
		${PARAVIEW_MOC_FILES}
	GUI_CONFIGURATION_XMLS
		${CMAKE_CURRENT_SOURCE_DIR}/ParaViewSources.xml
		${CMAKE_CURRENT_SOURCE_DIR}/ParaViewFilters.xml
)

# use C++11 to compile the client
set_property(TARGET paraview_stupro PROPERTY CXX_STANDARD 11)

# make sure not to copy or delete the resource folder if we are doing an in-source build
if(NOT ("${CMAKE_SOURCE_DIR}" STREQUAL "${KRONOS_BIN_DIR}"))
	# copy over all required resources after each build
	add_custom_command(
		TARGET paraview_stupro
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${KRONOS_BIN_DIR}/res
	)
	# delete the resource folder when executing make clean
	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${KRONOS_BIN_DIR}/res")
endif()

if(${PRINT_CLANG_COMPLETE})
	message(STATUS ".clang_complete include parameters")

	# get all include directories and print them
	get_directory_property(INCLUDE_DIRECTORIES INCLUDE_DIRECTORIES)
	list(REMOVE_DUPLICATES INCLUDE_DIRECTORIES)
	list(SORT INCLUDE_DIRECTORIES)
	foreach(ITEM ${INCLUDE_DIRECTORIES})
		message(STATUS "-I${ITEM}")
	endforeach()
endif()
