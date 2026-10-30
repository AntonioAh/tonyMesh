include(GoogleTest)

add_executable(tonyMesh_tests
        core/connectivityTest.cpp
)

target_link_libraries(tonyMesh_tests PRIVATE tonyMesh::core GTest::gtest_main)

tony_mesh_configure_target(tonyMesh_tests)
gtest_discover_tests(tonyMesh_tests)