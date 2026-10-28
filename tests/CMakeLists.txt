include(GoogleTest)

add_executable(tonyMesh_tests
        core/test_simulation.cpp
)

target_link_libraries(tonyMesh_tests PRIVATE sfs::core GTest::gtest_main)

sfs_configure_target(tonyMesh_tests)
gtest_discover_tests(tonyMesh_tests)