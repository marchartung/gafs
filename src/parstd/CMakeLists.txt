
cmake_minimum_required(VERSION 3.10)

 SET(SOURCES 
  apply.hpp
  enumerate.hpp
  exclusive_scan.hpp
  execution.hpp
  for_each_index.hpp
  fill.hpp
  merge.hpp
  ranges.hpp
  reduce.hpp
  sort.hpp
  unique.hpp
  vector.hpp
 )


SET(LIBRARIES "${LIBRARIES}")


add_library(gafs_parstd ${SOURCES})
set_target_properties(gafs_parstd PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(gafs_parstd PRIVATE cxx_std_20)
target_link_libraries(gafs_parstd ${LIBRARIES})