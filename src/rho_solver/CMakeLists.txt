
cmake_minimum_required(VERSION 3.10)

SET(SOURCES 
  rho_solver.cpp
  materials.hpp
  basic_equations.hpp
  dynamic_boundary.hpp
  particles.hpp
  neighbor.hpp
  std_rhs.hpp
)

SET(LIBRARIES "${LIBRARIES}" gafs_utils gafs_file)
add_executable(gafs_rho_solver ${SOURCES})
target_compile_features(gafs_rho_solver PRIVATE cxx_std_20)
target_include_directories(gafs_rho_solver PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(gafs_rho_solver ${LIBRARIES})