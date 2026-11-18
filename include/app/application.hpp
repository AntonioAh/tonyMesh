#pragma once
#include <memory.h>

#include <tonyMesh/app/rendered.hpp>
#include <tonyMesh/app/resourceManager.hpp>
namespace tonyMesh::app {

class Application {
public:
    explicit Application();
    void run();

    const Renderer& renderer() const { return m_renderer; }
    Renderer& renderer() { return m_renderer; }

    const ResourceManager& resManager() const { return m_resources; }
    ResourceManager& resManager() { return m_resources; }

private:
    Renderer m_renderer;
    ResourceManager m_resources;
};

}  // namespace tonyMesh::app
