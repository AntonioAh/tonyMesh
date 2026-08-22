cmake_minimum_required(VERSION 3.28)
project(tonyMesh LANGUAGES CXX C)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

include(FetchContent)

set(BUILD_TESTING OFF CACHE INTERNAL "")
set(EIGEN_BUILD_DOC OFF CACHE INTERNAL "")
set(EIGEN_BUILD_PKGCONFIG OFF CACHE INTERNAL "")

FetchContent_Declare(
        eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 5.0.0
)
FetchContent_MakeAvailable(eigen)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        libigl
        GIT_REPOSITORY https://github.com/libigl/libigl.git
        GIT_TAG v2.4.0
)
FetchContent_MakeAvailable(libigl)

enable_testing()
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)


add_library(tonyMesh src/glad.c
    src/mesh.cpp
    src/reconstruct.cpp
    src/normals.cpp
    src/pca.cpp
    src/dataStructures.cpp
)


target_include_directories(tonyMesh PUBLIC include)
target_link_libraries(tonyMesh PUBLIC Eigen3::Eigen glfw OpenGL::GL igl::core)
target_compile_definitions(tonyMesh PUBLIC GL_SILENCE_DEPRECATION)

add_executable(tony src/main.cpp)
target_link_libraries(tony PRIVATE tonyMesh)

#add_executable(tests tests/loaderSaver.cpp
#        include/pca.hpp)
#target_link_libraries(tests PRIVATE tonyMesh gtest_main)
#include(GoogleTest)
#gtest_discover_tests(tests)
##
