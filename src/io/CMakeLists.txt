
cmake_minimum_required(VERSION 3.10)

 SET(SOURCES 
    write_domain.hpp write_domain.cpp
 )

 SET(LIBRARIES "${LIBRARIES}" gafs_utils gafs_wsph gafs_file)

add_library(gafs_io ${SOURCES})

set_target_properties(gafs_io PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(gafs_io PRIVATE cxx_std_20)
target_include_directories(gafs_io PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(gafs_io ${LIBRARIES})