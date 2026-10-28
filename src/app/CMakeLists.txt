add_executable(tony_mesh_app
    main.cpp
        resourceManager.cpp
)

target_include_directories(tony_mesh_app PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(tony_mesh_app PRIVATE tonyMesh::core)

tony_mesh_configure_target(tony_mesh_app)
