
cmake_minimum_required(VERSION 3.10)

 SET(SOURCES 
  coords.hpp
  point_cell_list.hpp
  saved_neighbors.hpp saved_neighbors.cpp
 )


SET(LIBRARIES "${LIBRARIES}" gafs_utils gafs_algo)


add_library(gafs_neighbor ${SOURCES})
set_target_properties(gafs_neighbor PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(gafs_neighbor PRIVATE cxx_std_20)
target_include_directories(gafs_neighbor PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(gafs_neighbor ${LIBRARIES})