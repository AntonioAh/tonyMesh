#pragma once
#include <GLFW/glfw3.h>
#include <memory.h>

#include <tonyMesh/app/camera.hpp>
#include <tonyMesh/app/object.hpp>
#include <tonyMesh/app/rendered.hpp>
#include <tonyMesh/app/resourceManager.hpp>
#include <vector>
namespace tonyMesh::app {

class Application {
public:
    explicit Application() = default;
    void start();
    void run();

    const Renderer& renderer() const { return m_renderer; }
    Renderer& renderer() { return m_renderer; }

    const ResourceManager& resManager() const { return m_resources; }
    ResourceManager& resManager() { return m_resources; }

private:
    Renderer m_renderer;
    ResourceManager m_resources;
    Camera m_camera;
    std::vector<Object> m_objects;  // each one points to a mesh in m_resources

    GLFWwindow* m_window{};
    void m_readMouse();
    void m_drawObjectsPanel();

    Mesh* t_loadCube() { return m_resources.addMesh("cube", "cube.off"); }
};

}  // namespace tonyMesh::app
