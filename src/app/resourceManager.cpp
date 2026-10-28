#include <app/resourceManager.hpp>

namespace tonyMesh::app {
    std::shared_ptr<Mesh> ResourceManager::load(const std::string& name, std::shared_ptr<Mesh> mesh) {
        {
            std::shared_lock lock(m_mutexMesh);
            if (const auto it{m_meshMap.find(name)}; it != m_meshMap.end())
                return it->second;
        }

        std::unique_lock lock(m_mutexMesh);
        if (const auto it{m_meshMap.find(name)}; it != m_meshMap.end())
            return it->second;

        m_meshMap[name] = mesh;
        return mesh;
    }

    std::shared_ptr<Mesh> ResourceManager::getMesh(const std::string& name) const {
        std::shared_lock lock(m_mutexMesh);
        if (const auto it{m_meshMap.find(name)}; it != m_meshMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    void ResourceManager::unloadMesh(const std::string& name) {
        std::unique_lock lock(m_mutexMesh);
        m_meshMap.erase(name);
    }

    void ResourceManager::clear() {
        std::unique_lock lock(m_mutexMesh);
        m_meshMap.clear();
    }
}
