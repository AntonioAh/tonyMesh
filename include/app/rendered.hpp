#pragma once
#include <tonyMesh/app/shader.hpp>
#include <unordered_map>

namespace tonyMesh::app {
class Renderer {
public:
    explicit Renderer() = default;

    // Owns GPU objects: a copy would share the same GL handles.
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

private:
    std::unordered_map<std::string, Shader> m_shaders;
};
}  // namespace tonyMesh::app
