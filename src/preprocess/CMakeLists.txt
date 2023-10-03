
cmake_minimum_required(VERSION 3.10)

SET(SOURCES 
  point_shapes.hpp
  mesh_shapes.hpp
  cases.hpp cases.cpp
  solid_discretize.hpp
)


SET(LIBRARIES "${LIBRARIES}" gafs_utils gafs_wsph)

add_library(gafs_preprocess ${SOURCES})
target_compile_features(gafs_preprocess PRIVATE cxx_std_20)

target_include_directories(gafs_preprocess PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(gafs_preprocess ${LIBRARIES})