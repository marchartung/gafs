
cmake_minimum_required(VERSION 3.10)

set(SOURCES 
  basic_algorithms.hpp
  morton.hpp
  morton_octree.hpp morton_octree.cpp
  morton_points.hpp morton_points.cpp
)

add_library(gafs_algo ${SOURCES})
target_compile_features(gafs_algo PRIVATE cxx_std_20)
target_include_directories(gafs_algo PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(gafs_algo ${LIBRARIES} gafs_utils)