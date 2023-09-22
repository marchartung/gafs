

find_package(benchmark QUIET)
if(benchmark_FOUND)
    message("*INFO: benchmarks enabled")
    set(TEST_SOURCES 
        main.cpp
        helper_cpu_bench.hpp
        morton_bench.cpp
        dynamic_array_bench.cpp
    )

    add_executable(bench ${TEST_SOURCES})
    target_include_directories(bench PUBLIC ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(bench benchmark::benchmark_main benchmark::benchmark ${LIBRARIES} gafs_algo gafs_utils)
    target_compile_features(bench PRIVATE cxx_std_20)
elseif()
    message("*INFO: benchmarks disabled")
endif()