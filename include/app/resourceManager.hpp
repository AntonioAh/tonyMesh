#pragma once

#include <tonyMesh/app/shader.hpp>
#include <memory>
#include <tonyMesh/mesh.hpp>
#include <unordered_map>
namespace tonyMesh::app {
class ResourceManager {
public:
    ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

private:
    std::unordered_map<std::string, std::unique_ptr<Mesh>> m_meshes;
};

}  // namespace tonyMesh::app
