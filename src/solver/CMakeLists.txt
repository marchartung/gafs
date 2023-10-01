
cmake_minimum_required(VERSION 3.10)

SET(SOURCES 
  solver.cpp
)

SET(LIBRARIES "${LIBRARIES}" gafs_preprocess gafs_wsph gafs_io)
add_executable(gafs_solver ${SOURCES})
target_compile_features(gafs_solver PRIVATE cxx_std_20)
target_include_directories(gafs_solver PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(gafs_solver ${LIBRARIES})