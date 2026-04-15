cmake_minimum_required(VERSION 3.16)

project(tonymesh LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Eigen3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(glad src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(main)

target_sources(main 
  PRIVATE
    src/main.cpp 
    src/IO/meshLoader.cpp
    src/viewer/viewer.cpp
)

target_link_libraries(main
  PRIVATE
    glad
    glfw
    OpenGL::GL
    Eigen3::Eigen
)


if(APPLE)
  # OpenMP macOS (Homebrew)
  set(OPENMP_ROOT "/opt/homebrew/opt/libomp")
  target_include_directories(main PRIVATE ${OPENMP_ROOT}/include)
  target_compile_options(main PRIVATE -Xpreprocessor -fopenmp)
  target_link_libraries(main PRIVATE ${OPENMP_ROOT}/lib/libomp.dylib)
  message("done OpenMP for APPLE")

else()
    # Linux / Windows
  find_package(OpenMP REQUIRED)
  target_link_libraries(main PRIVATE OpenMP::OpenMP_CXX)

endif()
