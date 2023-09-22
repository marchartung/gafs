
cmake_minimum_required(VERSION 3.10)

SET(SOURCES 
  point_shapes.hpp
)


add_library(gafs_preprocess ${SOURCES})
target_compile_features(gafs_preprocess PRIVATE cxx_std_20)
target_link_libraries(gafs_preprocess ${LIBRARIES})