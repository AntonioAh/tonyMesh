#pragma once

#include <core/mesh.hpp>
#include <unordered_map>
#include <memory>
#include <shared_mutex>

namespace tonyMesh::app {

class ResourceManager {
public:
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    static ResourceManager& getInstance() {
        static ResourceManager manager;
        return manager;
    }

    std::shared_ptr<Mesh> load(const std::string& name, std::shared_ptr<Mesh> mesh);
    std::shared_ptr<Mesh> getMesh(const std::string& name) const;
    void unloadMesh(const std::string& name);
    void clear();


private:
    ResourceManager() = default;
    ~ResourceManager() = default;

    mutable std::shared_mutex m_mutexMesh;
    std::unordered_map<std::string, std::shared_ptr<Mesh>> m_meshMap;
};
}