#include <spdlog/spdlog.h>

#include <tonyMesh/app/camera.hpp>
#include <tonyMesh/app/rendered.hpp>
#include <tonyMesh/app/shader.hpp>

namespace tonyMesh::app {
void Renderer::init() {
    m_shaders.try_emplace("mesh", "mesh.vert", "mesh.frag");
    spdlog::info("loaded shader mesh");
}

void Renderer::release() {
    m_gpuMeshes.clear();
    m_shaders.clear();
}

void Renderer::draw(const std::vector<Object>& objects, const Camera& camera,
                    float aspect) {
    Shader& shader = m_shaders.at("mesh");
    shader.use();
    shader.setMat4("view", camera.view());
    shader.setMat4("projection", camera.projection(aspect));

    for (const Object& object : objects) {
        auto it = m_gpuMeshes.find(&object.mesh());
        if (it ==
            m_gpuMeshes.end())  // first time this mesh is drawn: upload it
            it = m_gpuMeshes.try_emplace(&object.mesh(), object.mesh()).first;

        shader.setMat4("model", object.model().matrix());
        shader.setVec3("color", object.color());
        it->second.draw();
    }
}
}  // namespace tonyMesh::app
