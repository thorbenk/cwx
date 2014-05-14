project(cwx)

# cmake setup
cmake_minimum_required(VERSION 2.8)
list(APPEND CMAKE_MODULE_PATH "${cwx_SOURCE_DIR}/config")
enable_testing()

# C++11
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
add_definitions(-DHAVE_CPP11_INITIALIZER_LISTS)

# packages
find_package(HDF5 COMPONENTS C HL REQUIRED)
find_package(Valgrind)

include_directories(
    deps/marray/include/andres
    deps/stack-vector/include/andres
    include
    include/andres
    ${HDF5_INCLUDE_DIRS}
)

add_subdirectory(cmd)
add_subdirectory(test)
if(VALGRIND_FOUND)
    include_directories(${VALGRIND_INCLUDE_DIR})
    add_subdirectory(bench)
endif()

