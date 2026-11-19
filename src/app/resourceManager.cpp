#include <spdlog/spdlog.h>

#include <mutex>
#include <tonyMesh/IO.hpp>
#include <tonyMesh/app/resourceManager.hpp>

namespace tonyMesh::app {
// If `name` already exists the existing mesh is kept and returned (load once).
Mesh* ResourceManager::addMesh(std::string name, std::unique_ptr<Mesh> mesh) {
    if (!mesh) {
        spdlog::error("cannot add mesh {}: null mesh", name);
        return nullptr;
    }

    std::unique_lock lock(m_mutex);
    auto [it, inserted] =
        m_meshes.try_emplace(std::move(name), std::move(mesh));
    return it->second.get();
}

Mesh* ResourceManager::addMesh(std::string name, std::string_view filePath) {
    if (Mesh* existing = mesh(name)) return existing;

    auto loaded = tonyMesh::IO::readMesh(filePath);
    if (!loaded) {
        spdlog::error("cannot load mesh {} from {}", name, filePath);
        return nullptr;
    }
    return addMesh(std::move(name), std::make_unique<Mesh>(std::move(*loaded)));
}

const Mesh* ResourceManager::mesh(std::string_view name) const {
    std::shared_lock lock(m_mutex);
    auto found = m_meshes.find(name);
    return found != m_meshes.end() ? found->second.get() : nullptr;
}

Mesh* ResourceManager::mesh(std::string_view name) {
    std::shared_lock lock(m_mutex);
    auto found = m_meshes.find(name);
    return found != m_meshes.end() ? found->second.get() : nullptr;
}

std::unique_ptr<Mesh> ResourceManager::removeMesh(std::string_view name) {
    std::unique_lock lock(m_mutex);
    auto found = m_meshes.find(name);
    if (found == m_meshes.end()) return nullptr;

    auto removed = std::move(found->second);
    m_meshes.erase(found);
    return removed;  // the caller destroys the mesh, after the lock is released
}
}  // namespace tonyMesh::app
