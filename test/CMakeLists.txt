

SET(SOURCES 
  # memory_test.cpp 
  # morton_test.cpp
  parstd/vector_test.cpp
  neighbor/saved_neighbors_test.cpp
  neighbor/point_cell_list_test.cpp
  wsph/basic_equations_test.cpp
)

find_package(GTest QUIET)
if(GTest_FOUND)
  message("*INFO: tests enabled")
  add_executable(test ${SOURCES})
  target_include_directories(test PUBLIC ${PROJECT_SOURCE_DIR}/src)
  target_compile_features(test PRIVATE cxx_std_20)
  target_link_libraries(test GTest::gtest GTest::gtest_main ${LIBRARIES} gafs_neighbor gafs_utils gafs_algo)
  target_compile_features(test INTERFACE cxx_std_20)
elseif()
  message("*INFO: tests disabled")
endif()
