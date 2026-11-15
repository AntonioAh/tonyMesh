# --- Public include prefix ----------------------------------------------------
# Headers live in include/core/ but are included as <tonyMesh/...>.
# A symlink build/<preset>/include/tonyMesh -> include/core provides the prefix.
set(tonyMesh_gen_include "${PROJECT_BINARY_DIR}/include")
file(MAKE_DIRECTORY "${tonyMesh_gen_include}")
file(CREATE_LINK
    "${PROJECT_SOURCE_DIR}/include/core"
    "${tonyMesh_gen_include}/tonyMesh"
    SYMBOLIC)

# --- Core library -------------------------------------------------------------
# Sources are collected automatically; re-globbed at build time (CONFIGURE_DEPENDS).
file(GLOB_RECURSE tonyMesh_core_sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

if(NOT tonyMesh_core_sources)
    # No .cpp files yet: compile an empty generated TU so tonyMesh stays a real
    # STATIC library. IDEs need its compile flags to resolve includes in headers.
    set(tonyMesh_placeholder "${PROJECT_BINARY_DIR}/generated/tonyMesh_placeholder.cpp")
    file(CONFIGURE OUTPUT "${tonyMesh_placeholder}"
        CONTENT "// Generated: placeholder until src/core contains sources.\n")
    set(tonyMesh_core_sources "${tonyMesh_placeholder}")
endif()

add_library(tonyMesh STATIC ${tonyMesh_core_sources})
add_library(tonyMesh::tonyMesh ALIAS tonyMesh)

target_compile_definitions(tonyMesh PRIVATE TONYMESH_VERSION="${PROJECT_VERSION}")
target_link_libraries(tonyMesh PRIVATE tonyMesh_warnings)

# Register the public headers so IDEs (CLion) know they belong to tonyMesh
# and resolve their includes (Eigen, TBB, ...) even before a .cpp uses them.
file(GLOB_RECURSE tonyMesh_core_headers CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/include/core/*.hpp")

# Plain PRIVATE sources (not a FILE_SET): a FILE_SET's BASE_DIRS would put
# include/core on the include path and allow <mesh.hpp>; we want only <tonyMesh/mesh.hpp>.
target_sources(tonyMesh PRIVATE ${tonyMesh_core_headers})

target_include_directories(tonyMesh
    PUBLIC
        $<BUILD_INTERFACE:${tonyMesh_gen_include}>)

target_link_libraries(tonyMesh
    PUBLIC
        Eigen3::Eigen
        TBB::tbb)
