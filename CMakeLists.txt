cmake_minimum_required(VERSION 3.25)

project(tonyMesh
    VERSION 0.1.0
    DESCRIPTION "Mesh processing library"
    LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

option(TONYMESH_BUILD_TESTS "Build the tonyMesh test suite" ${PROJECT_IS_TOP_LEVEL})

# Warning flags applied only to our own targets (never to dependencies).
add_library(tonyMesh_warnings INTERFACE)
target_compile_options(tonyMesh_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Wconversion -Wshadow>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->)

include(cmake/Dependencies.cmake)

add_subdirectory(src)

if(TONYMESH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
