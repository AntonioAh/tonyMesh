#pragma once
#include <tonyMesh/app/shader.hpp>
#include <unordered_map>

namespace tonyMesh::app {
class Renderer {
public:
private:
    std::unordered_map<std::string, Shader> m_shaders;
};
}  // namespace tonyMesh::app
