
SET(SOURCES 
  vtk_helper.hpp vtk_helper.cpp
  vtp.hpp vtp.cpp
)

find_package(VTK COMPONENTS
  CommonCore IOXML
)
message(STATUS "VTK Version: ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}")
message(STATUS "VTK Libraries: ${VTK_LIBRARIES}")


add_library(gafs_file ${SOURCES})
target_compile_features(gafs_file PRIVATE cxx_std_20)
target_include_directories(gafs_file PRIVATE ${PROJECT_SOURCE_DIR}/src ${VTK_INCLUDE_DIRS})
target_link_libraries(gafs_file ${LIBRARIES} ${VTK_LIBRARIES})