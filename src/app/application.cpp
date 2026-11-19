#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <stdexcept>
#include <tonyMesh/app/application.hpp>

namespace tonyMesh::app {
void Application::run() {
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();

        // --- start a new ImGui frame ---
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        m_readMouse();
        m_drawObjectsPanel();

        // --- clear the screen ---
        int width{}, height{};
        glfwGetFramebufferSize(m_window, &width,
                               &height);  // pixels (2x on Retina)
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- draw the scene (after the clear, before the UI) ---
        const float aspect =
            height > 0 ? static_cast<float>(width) / static_cast<float>(height)
                       : 1.0f;
        m_renderer.draw(m_objects, m_camera, aspect);

        // --- draw the UI on top, then show the frame ---
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_window);
    }

    // shutdown, in reverse order of creation
    m_renderer.release();  // GL objects must be freed while the context exists
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Application::start() {
    if (!glfwInit()) throw std::runtime_error("glfwInit failed");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // required on macOS

    m_window = glfwCreateWindow(1280, 720, "Tony Mesh ", nullptr, nullptr);
    if (!m_window) throw std::runtime_error("cannot create the window");

    // The GL context must be current before glad loads functions and before any
    // gl* call.
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);  // vsync
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        throw std::runtime_error("cannot load OpenGL functions");
    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr;
    ImGui_ImplGlfw_InitForOpenGL(
        m_window, true);  // true: ImGui installs the input callbacks
    ImGui_ImplOpenGL3_Init("#version 410 core");

    renderer().init();

    Mesh* mesh = t_loadCube();
    if (!mesh) throw std::runtime_error("cannot load cube.off");

    // Three appearances of the same mesh: its data is stored (CPU and GPU) once.
    const Eigen::Vector3f size = (mesh->vertices().colwise().maxCoeff() -
                                  mesh->vertices().colwise().minCoeff())
                                     .transpose();
    const float spacing = 1.2f * size.x();
    const Eigen::Vector3f colors[] = {{0.9f, 0.4f, 0.3f},
                                      {0.8f, 0.8f, 0.8f},
                                      {0.3f, 0.6f, 0.9f}};
    for (int i = -1; i <= 1; ++i) {
        Object object(*mesh);
        object.position().x() = static_cast<float>(i) * spacing;
        object.color() = colors[i + 1];
        m_objects.push_back(object);
    }
    m_camera.frame(*mesh, 2.5f);  // margin: leave room for the side copies
}

void Application::m_drawObjectsPanel() {
    ImGui::Begin("Objects");
    for (std::size_t i = 0; i < m_objects.size(); ++i) {
        Object& object = m_objects[i];
        ImGui::PushID(static_cast<int>(i));
        ImGui::Text("Object %d", static_cast<int>(i));
        ImGui::DragFloat3("position", object.position().data(), 0.05f);
        ImGui::DragFloat("scale", &object.scale(), 0.01f, 0.01f, 100.f);
        ImGui::ColorEdit3("color", object.color().data());
        ImGui::Separator();
        ImGui::PopID();
    }
    ImGui::End();
}

void Application::m_readMouse() {
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse) {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            m_camera.orbit(io.MouseDelta.x, io.MouseDelta.y);
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
            m_camera.pan(io.MouseDelta.x, io.MouseDelta.y);
        if (io.MouseWheel != 0.0f) m_camera.zoom(io.MouseWheel);
    }
}
}  // namespace tonyMesh::app
