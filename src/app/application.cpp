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

        // ImGui::ShowDemoWindow();  // built-in test window, shows ImGui works

        // --- clear the screen ---
        int width{}, height{};
        glfwGetFramebufferSize(m_window, &width,
                               &height);  // pixels (2x on Retina)
        glViewport(0, 0, width, height);
        glClearColor(1.0f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // (the 3D scene will be drawn here later)

        // --- draw the UI on top, then show the frame ---
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_window);
    }

    // shutdown, in reverse order of creation
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
}
}  // namespace tonyMesh::app
