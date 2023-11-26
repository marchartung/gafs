
cmake_minimum_required(VERSION 3.10)

SET(SOURCES 
  atomic_wrapper.hpp
  cli.hpp cli.cpp
  macros.hpp
  math.hpp
  timer.hpp timer.cpp
  types.hpp
  random.hpp
  memory.hpp
)


add_library(gafs_utils ${SOURCES})
target_compile_features(gafs_utils PRIVATE cxx_std_20)
target_link_libraries(gafs_utils ${LIBRARIES})