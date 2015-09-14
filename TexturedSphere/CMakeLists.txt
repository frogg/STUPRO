
cmake_minimum_required(VERSION 2.8)

PROJECT(TexturedSphere)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(TexturedSphere MACOSX_BUNDLE TexturedSphere)

if(VTK_LIBRARIES)
  target_link_libraries(TexturedSphere ${VTK_LIBRARIES})
else()
  target_link_libraries(TexturedSphere vtkHybrid vtkWidgets)
endif()
