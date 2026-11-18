# --- tonyMesh application (GUI) ----------------------------------------------
# Window/GL dependencies are fetched here only, so they never leak into core or tests.
include(FetchContent)

# OpenGL is provided by the system (framework on macOS), not fetched.
find_package(OpenGL REQUIRED)

# --- GLFW: window, GL context, input -----------------------------------------
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
    GIT_SHALLOW    TRUE
    SYSTEM)

# --- glad: OpenGL function loader (generated at configure time, needs Python) -
set(GLAD_PROFILE   "core" CACHE STRING "" FORCE)
set(GLAD_API       "gl=4.1" CACHE STRING "" FORCE)
set(GLAD_GENERATOR "c" CACHE STRING "" FORCE)

FetchContent_Declare(glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG        v0.1.36
    GIT_SHALLOW    TRUE
    SYSTEM)

FetchContent_MakeAvailable(glfw)

# glad 0.1.36 declares cmake_minimum_required < 3.5, which CMake 4 rejects; scoped to glad only.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_MakeAvailable(glad)
unset(CMAKE_POLICY_VERSION_MINIMUM)

# --- Dear ImGui: immediate-mode GUI (no CMake upstream, built here) ----------
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.8
    GIT_SHALLOW    TRUE
    SYSTEM)
FetchContent_MakeAvailable(imgui)

add_library(tonyMesh_imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)

target_include_directories(tonyMesh_imgui SYSTEM
    PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends)

target_link_libraries(tonyMesh_imgui
    PUBLIC
        glfw)

# --- Executable ---------------------------------------------------------------
file(GLOB_RECURSE tonyMesh_app_sources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

if(NOT tonyMesh_app_sources)
    # No .cpp files yet: generate a stub main so the target still configures and links.
    set(tonyMesh_app_placeholder "${PROJECT_BINARY_DIR}/generated/tonyMesh_app_placeholder.cpp")
    file(CONFIGURE OUTPUT "${tonyMesh_app_placeholder}"
        CONTENT "// Generated: placeholder until src/app contains sources.\nint main() { return 0; }\n")
    set(tonyMesh_app_sources "${tonyMesh_app_placeholder}")
endif()

add_executable(tonyMesh_app ${tonyMesh_app_sources})

# Headers live in include/app/ but are included as <tonyMesh/app/...>.
# A symlink <build>/generated/include/tonyMesh/app -> include/app provides that include name.
set(tonyMesh_app_include_root "${PROJECT_BINARY_DIR}/generated/include")
file(MAKE_DIRECTORY "${tonyMesh_app_include_root}/tonyMesh")
file(CREATE_LINK "${PROJECT_SOURCE_DIR}/include/app"
    "${tonyMesh_app_include_root}/tonyMesh/app" SYMBOLIC)

target_include_directories(tonyMesh_app
    PRIVATE
        ${tonyMesh_app_include_root})

# Registered so IDEs attribute the headers to this target.
file(GLOB_RECURSE tonyMesh_app_headers CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/include/app/*.hpp")
target_sources(tonyMesh_app PRIVATE ${tonyMesh_app_headers})

target_link_libraries(tonyMesh_app
    PRIVATE
        tonyMesh::tonyMesh
        glfw
        glad
        tonyMesh_imgui
        OpenGL::GL
        tonyMesh_warnings)

# Apple marks OpenGL as deprecated; silence the per-call warnings.
# The shaders are read at startup from src/app/shaders/ (absolute path, so the working directory does not matter).
target_compile_definitions(tonyMesh_app
    PRIVATE
        TONYMESH_SHADER_DIR="${PROJECT_SOURCE_DIR}/src/app/shaders/"
        $<$<PLATFORM_ID:Darwin>:GL_SILENCE_DEPRECATION>)
