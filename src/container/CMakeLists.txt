
cmake_minimum_required(VERSION 3.10)

SET(SOURCES 
  pointer_ensured_vector.hpp
  dynamic_array.hpp
)


add_library(gafs_container ${SOURCES})
set_target_properties(gafs_container PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(gafs_container PRIVATE cxx_std_20)
target_link_libraries(gafs_container ${LIBRARIES})