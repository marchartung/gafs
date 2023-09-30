
cmake_minimum_required(VERSION 3.10)

SET(SOURCES 
  solver.cpp
  materials.hpp
  basic_equations.hpp basic_equations.cpp
  dynamic_boundary.hpp
  particles.hpp
  mesh.hpp
  volume_boundary.hpp
  time_stepping.hpp time_stepping.cpp
  derivatives.hpp derivatives.cpp
  domain.hpp
  shifting.hpp
)

SET(LIBRARIES "${LIBRARIES}" gafs_utils gafs_file)
add_executable(gafs_solver ${SOURCES})
target_compile_features(gafs_solver PRIVATE cxx_std_20)
target_include_directories(gafs_solver PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(gafs_solver gafs_neighbor ${LIBRARIES})