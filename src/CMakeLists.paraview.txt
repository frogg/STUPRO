cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)
project(ParaViewCustomApplication)

message(STATUS ${CMAKE_MODULE_PATH})

set (ParaView_DIR "/paraview-bin/"
      CACHE PATH
      "Path to ParaView build")
      
if(EXISTS "${ParaView_DIR}/ParaViewConfig.cmake" OR EXISTS "${ParaView_DIR}/paraview-config.cmake")
   find_package(ParaView REQUIRED)
else()
   message (FATAL_ERROR
	"ParaView not found, enter the path "
	"to a ParaView build dir in ParaView_DIR")
endif()
   
include(${PARAVIEW_USE_FILE})
if (NOT PARAVIEW_BUILD_QT_GUI)
  message(FATAL_ERROR
    "PV Custom App requires PARAVIEW_BUILD_QT_GUI to be enabled. "
    "Please rebuild ParaView (or point to a different build of ParaView) "
    "with PARAVIEW_BUILD_QT_GUI set to TRUE")
endif()

include(ParaViewBranding)

#------------------------------------------------------------------------------
# Add extra library containing custom code for the client.
IF (PARAVIEW_QT_VERSION VERSION_GREATER "4")
  QT5_WRAP_CPP(MOC_BUILT_SOURCES
    src/StuproWindow.h)
  QT5_WRAP_UI(UI_BUILT_SOURCES
    src/StuproWindow.ui)
ELSE ()
  QT4_WRAP_CPP(MOC_BUILT_SOURCES
    src/StuproWindow.h)
  QT4_WRAP_UI(UI_BUILT_SOURCES
    src/StuproWindow.ui)
ENDIF ()

SOURCE_GROUP("Generated" FILES
  ${MOC_BUILT_SOURCES}
  ${UI_BUILT_SOURCES}
)

SET(ParaView_SOURCE_FILES
  src/StuproWindow.cxx
  src/StuproWindow.h
  ${MOC_BUILT_SOURCES}
  ${UI_BUILT_SOURCES})

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/StuproView
  ${CMAKE_CURRENT_BINARY_DIR})

# Add required source-files for the StuproView-plugin
FILE(GLOB StuproView_SOURCE_FILES
  "src/StuproView/*.cpp"
  "src/StuproView/*.hpp")

#Required for the ParaView plugin
add_paraview_plugin(
  StuproViewPlugin "1.0"
  REQUIRED_ON_SERVER
  REQUIRED_ON_CLIENT
  SERVER_MANAGER_SOURCES
    src/StuproView/vtkPVStuproView.cxx
  SERVER_MANAGER_XML
    src/StuproView/StuproView.xml
  SOURCES
	${StuproView_SOURCE_FILES}
)

#Copy the needed files to the output
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/StuproView/Shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/StuproView/Resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/StuproView/Tiles DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

build_paraview_client(paraview_stupro
  TITLE "ParaView - STUPRO-Version"
  ORGANIZATION  "Kitware Inc."
  VERSION_MAJOR 1 
  VERSION_MINOR 1
  VERSION_PATCH 1
  PVMAIN_WINDOW StuproWindow
  PVMAIN_WINDOW_INCLUDE StuproWindow.h
  REQUIRED_PLUGINS StuproViewPlugin
  #GUI_CONFIGURATION_XMLS
  #  ${CMAKE_CURRENT_SOURCE_DIR}/src/ParaViewSources.xml
  #  ${CMAKE_CURRENT_SOURCE_DIR}/src/ParaViewFilters.xml
  #  ${CMAKE_CURRENT_SOURCE_DIR}/src/ParaViewWriters.xml
  SOURCES ${ParaView_SOURCE_FILES}
)
