#pragma once

#include <functional>
#include <memory>
#include <shared_mutex>
#include <string>
#include <string_view>
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

    Mesh* addMesh(std::string name, std::string_view filePath);
    Mesh* addMesh(std::string name, std::unique_ptr<Mesh> mesh);
    const Mesh* mesh(std::string_view name) const;
    Mesh* mesh(std::string_view name);
    std::unique_ptr<Mesh> removeMesh(std::string_view name);

private:
    struct StringHash {
        using is_transparent = void;  // enables lookup with string_view
        std::size_t operator()(std::string_view s) const noexcept {
            return std::hash<std::string_view>{}(s);
        }
    };

    mutable std::shared_mutex m_mutex;
    std::unordered_map<std::string, std::unique_ptr<Mesh>, StringHash,
                       std::equal_to<>>
        m_meshes;
};

}  // namespace tonyMesh::app
