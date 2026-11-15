# GoogleTest and libigl are fetched here only, so it never leaks into core or app targets.
file(GLOB_RECURSE tonyMesh_test_sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

if(NOT tonyMesh_test_sources)
    message(STATUS "tonyMesh: no test sources found, skipping tests")
    return()
endif()

include(FetchContent)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK   OFF CACHE BOOL "" FORCE)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.18.0
    GIT_SHALLOW    TRUE
    SYSTEM)
FetchContent_MakeAvailable(googletest)

# libigl: ground truth for the mesh readers. Header-only, so its templates
# instantiate with tonyMesh's own types (Real = float). Only the core module.
foreach(opt
        LIBIGL_USE_STATIC_LIBRARY LIBIGL_BUILD_TESTS LIBIGL_BUILD_TUTORIALS LIBIGL_INSTALL
        LIBIGL_EMBREE LIBIGL_GLFW LIBIGL_IMGUI LIBIGL_OPENGL LIBIGL_PNG LIBIGL_PREDICATES
        LIBIGL_SPECTRA LIBIGL_XML
        LIBIGL_COPYLEFT_CORE LIBIGL_COPYLEFT_CGAL LIBIGL_COPYLEFT_COMISO LIBIGL_COPYLEFT_TETGEN
        LIBIGL_RESTRICTED_TRIANGLE LIBIGL_RESTRICTED_MATLAB LIBIGL_RESTRICTED_MOSEK)
    set(${opt} OFF CACHE BOOL "" FORCE)
endforeach()

FetchContent_Declare(libigl
    GIT_REPOSITORY https://github.com/libigl/libigl.git
    GIT_TAG        v2.6.0
    GIT_SHALLOW    TRUE
    SYSTEM)
FetchContent_MakeAvailable(libigl)

add_executable(tonyMesh_tests ${tonyMesh_test_sources})

target_link_libraries(tonyMesh_tests
    PRIVATE
        tonyMesh::tonyMesh
        GTest::gtest_main
        igl::core
        tonyMesh_warnings)

include(GoogleTest)
# Run from the repo root so the relative "data/" search folder resolves.
gtest_discover_tests(tonyMesh_tests
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
