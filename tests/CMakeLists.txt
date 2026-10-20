cmake_minimum_required(VERSION 3.25...4.4)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
        GIT_SHALLOW TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(LIBIGL_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LIBIGL_BUILD_TUTORIALS OFF CACHE BOOL "" FORCE)
set(LIBIGL_BUILD_PYTHON OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
        libigl
        GIT_REPOSITORY https://github.com/libigl/libigl.git
        GIT_TAG v2.5.0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(libigl)

include(GoogleTest)

add_executable(tonyTests
        connettivity_test.cpp
)

target_link_libraries(tonyTests PRIVATE
        tonyMesh
        igl::core
        GTest::gtest_main
)

file(GLOB DATA_FILES "${CMAKE_SOURCE_DIR}/data/*")
foreach(DATA_FILE ${DATA_FILES})
    get_filename_component(FILE_NAME ${DATA_FILE} NAME)
    file(CREATE_LINK "${DATA_FILE}" "${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}" SYMBOLIC)
endforeach()

gtest_discover_tests(tonyTests
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data"
)