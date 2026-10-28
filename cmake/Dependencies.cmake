include(FetchContent)
find_package(Git REQUIRED)

FetchContent_Declare(eigen
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG        5.0.1
        GIT_SHALLOW    TRUE
        SYSTEM)
FetchContent_MakeAvailable(eigen)

block()
    set(BUILD_SHARED_LIBS ON)
    set(TBB_TEST OFF)
    set(TBB_EXAMPLES OFF)
    set(TBB_INSTALL OFF)
    set(TBB_STRICT OFF)
    set(TBBMALLOC_PROXY_BUILD OFF)
    set(TBB_DISABLE_HWLOC_AUTOMATIC_SEARCH ON)
    FetchContent_Declare(tbb
            URL      https://github.com/uxlfoundation/oneTBB/archive/refs/tags/v2023.1.0.tar.gz
            URL_HASH SHA256=191288b52e1e6b17198000b64d77d194bb65e791be46ebc606e9b091781e2070
            SYSTEM)
    FetchContent_MakeAvailable(tbb)
endblock()

if(TONY_MESH_BUILD_TESTS)
    set(INSTALL_GTEST OFF)
    set(gtest_force_shared_crt ON)
    FetchContent_Declare(googletest
            URL      https://github.com/google/googletest/archive/refs/tags/v1.18.0.tar.gz
            URL_HASH SHA256=6e3191c1455468b3fc35a417fb565c1c5071aee1b7e7f85e30cf48a98d37d8b5
            SYSTEM)
    FetchContent_MakeAvailable(googletest)
endif()
