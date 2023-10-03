
cmake_minimum_required(VERSION 3.10)

SET(SOURCES 
  materials.hpp
  basic_equations.hpp basic_equations.cpp
  particles.hpp
  mesh.hpp
  time_stepping.hpp time_stepping.cpp
  derivatives.hpp derivatives.cpp
  domain.hpp
  shifting.hpp shifting.cpp
  particle_boundary.hpp
)

SET(LIBRARIES "${LIBRARIES}" gafs_utils gafs_neighbor)


add_library(gafs_wsph ${SOURCES})
target_compile_features(gafs_wsph PRIVATE cxx_std_20)
target_include_directories(gafs_wsph PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(gafs_wsph  ${LIBRARIES})