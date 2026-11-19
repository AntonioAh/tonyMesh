#pragma once
#include <string>
#include <tonyMesh/app/camera.hpp>
#include <tonyMesh/app/gpuMesh.hpp>
#include <tonyMesh/app/object.hpp>
#include <tonyMesh/app/shader.hpp>
#include <unordered_map>
#include <vector>

namespace tonyMesh::app {
class Renderer {
public:
    explicit Renderer() = default;

    // Owns GPU objects: a copy would share the same GL handles.
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void init();     // compiles the shaders; needs a current GL context
    void release();  // frees every GL object; call while the context is alive

    // One draw call per object; each distinct Mesh is uploaded to the GPU once.
    void draw(const std::vector<Object>& objects, const Camera& camera,
              float aspect);

private:
    std::unordered_map<std::string, Shader> m_shaders;
    std::unordered_map<const Mesh*, GpuMesh> m_gpuMeshes;
};
}  // namespace tonyMesh::app
