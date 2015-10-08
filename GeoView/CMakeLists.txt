
cmake_minimum_required(VERSION 2.8)

PROJECT(GeoView)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(GeoView MACOSX_BUNDLE GeoView)

if(VTK_LIBRARIES)
  target_link_libraries(GeoView ${VTK_LIBRARIES})
else()
  target_link_libraries(GeoView vtkHybrid vtkWidgets)
endif()
