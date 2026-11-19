# Third-party dependencies for the tonyMesh library and app.
# NOTE: GoogleTest is intentionally NOT here; it is fetched in tests/CMakeLists.txt only.

include(FetchContent)

# Keep dependencies from registering their own tests/docs.
set(BUILD_TESTING OFF)

# --- Eigen ------------------------------------------------------------------
set(EIGEN_BUILD_TESTING   OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_DOC       OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_PKGCONFIG OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_BLAS      OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_LAPACK    OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_DEMOS     OFF CACHE BOOL "" FORCE)

FetchContent_Declare(Eigen3
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG        5.0.1
    GIT_SHALLOW    TRUE
    SYSTEM)

# --- oneTBB -----------------------------------------------------------------
set(TBB_TEST     OFF CACHE BOOL "" FORCE)
set(TBB_EXAMPLES OFF CACHE BOOL "" FORCE)
set(TBB_STRICT   OFF CACHE BOOL "" FORCE)
set(TCM_BUILD    OFF CACHE BOOL "" FORCE)  # optional Thread Composability Manager

FetchContent_Declare(TBB
    GIT_REPOSITORY https://github.com/uxlfoundation/oneTBB.git
    GIT_TAG        v2023.2.0
    GIT_SHALLOW    TRUE
    SYSTEM)

FetchContent_MakeAvailable(Eigen3 TBB)

# --- spdlog -----------------------------------------------------------------
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.x
    GIT_SHALLOW TRUE
    SYSTEM)
FetchContent_MakeAvailable(spdlog)

unset(BUILD_TESTING)
