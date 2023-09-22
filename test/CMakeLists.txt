

find_package(GTest QUIET)
if(GTest_FOUND)
  message("*INFO: tests enabled")
  add_executable(test memory_test.cpp morton_test.cpp basic_algorithms_test.cpp)
  target_include_directories(test PUBLIC ${PROJECT_SOURCE_DIR}/src)
  target_compile_features(test PRIVATE cxx_std_20)
  target_link_libraries(test GTest::gtest GTest::gtest_main ${LIBRARIES} gafs_utils gafs_algo)
  target_compile_features(test INTERFACE cxx_std_20)
elseif()
  message("*INFO: tests disabled")
endif()
