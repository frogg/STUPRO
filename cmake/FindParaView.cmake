#
# Find the ParaView includes and library
#

set(PARAVIEW_FOUND "NO")

set(ParaView_DIR "../bin/paraview/bin/"
    CACHE PATH
    "Path to ParaView build")

if(EXISTS "${ParaView_DIR}/ParaViewConfig.cmake" OR EXISTS "${ParaView_DIR}/paraview-config.cmake")
    find_package(ParaView REQUIRED)
else()
    message (FATAL_ERROR
    "ParaView not found, enter the path "
    "to a ParaView build dir in ParaView_DIR")
endif()

if (NOT PARAVIEW_BUILD_QT_GUI)
    message(FATAL_ERROR
        "PV Custom App requires PARAVIEW_BUILD_QT_GUI to be enabled. "
        "Please rebuild ParaView (or point to a different build of ParaView) "
        "with PARAVIEW_BUILD_QT_GUI set to TRUE")
endif()
