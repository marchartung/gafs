
project(Gafs)
project(Gafs LANGUAGES CXX)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT DEFINED ROCM_PATH )
     set ( ROCM_PATH "/opt/rocm"  CACHE STRING "Default ROCM installation directory." )
endif ()
list (APPEND CMAKE_PREFIX_PATH /opt/rocm/hip /opt/rocm)
list(APPEND CMAKE_PREFIX_PATH ${ROCM_PATH}/hip ${ROCM_PATH})

find_package(hip QUIET)
if(NOT DEFINED GPU_DISABLE AND hip_FOUND)
    message(STATUS "HIP found using ${HIP_HIPCC_EXECUTABLE} as compiler")
    find_package(hipcub  REQUIRED CONFIG PATHS "/opt/rocm/hipcub")
    find_package(rocprim REQUIRED CONFIG PATHS "/opt/rocm/rocprim")
    find_package(rocthrust REQUIRED CONFIG PATHS "/opt/rocm/rocthrust")
    
    # Set compiler and linker
    set(CMAKE_CXX_COMPILER ${HIP_HIPCC_EXECUTABLE})
    set(CMAKE_CXX_LINKER   ${HIP_HIPCC_EXECUTABLE})

    
    add_compile_definitions(OMP_ENABLED=1)
    add_compile_definitions(GPU_ENABLED=1)
    add_compile_definitions(__HIP__=1)

    message(STATUS "OMP LIBRARIES: Using hard linking due to HIP")
    set(LIBRARIES ${LIBRARIES} hip::host gomp tbb)
else()
    message(STATUS "HIP not found or disabled")
    find_package(OpenMP REQUIRED)
    set(LIBRARIES ${LIBRARIES} OpenMP::OpenMP_CXX)
    add_compile_definitions(OMP_ENABLED=1)
    message(STATUS "OMP LIBRARIES: ${OpenMP_CXX_LIBRARIES}")
endif()


find_package(TBB QUIET)
if (TBB_FOUND)
    set(LIBRARIES ${LIBRARIES} TBB::tbb)
    add_compile_definitions(TBB_ENABLED=1)
    message(STATUS "TBB found")
endif()
    



add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(benchmark)
