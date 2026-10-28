add_library(tony_mesh_core STATIC mesh.cpp)

add_library(tonyMesh::core ALIAS tony_mesh_core)

target_include_directories(tony_mesh_core PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(tony_mesh_core PUBLIC cxx_std_20)
target_link_libraries(tony_mesh_core PUBLIC Eigen3::Eigen TBB::tbb)

tony_mesh_configure_target(tony_mesh_core)