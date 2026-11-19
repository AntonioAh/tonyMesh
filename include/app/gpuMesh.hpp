#pragma once

namespace tonyMesh {
class Mesh;
}

namespace tonyMesh::app {

// GPU copy of a Mesh: one VAO + VBO (position, normal) + EBO. Move-only.
class GpuMesh {
public:
    explicit GpuMesh(const Mesh& mesh);  // needs a current GL context
    ~GpuMesh();

    GpuMesh(const GpuMesh&) = delete;
    GpuMesh& operator=(const GpuMesh&) = delete;
    GpuMesh(GpuMesh&& other) noexcept;
    GpuMesh& operator=(GpuMesh&& other) noexcept;

    void draw() const;

private:
    unsigned m_vao{}, m_vbo{}, m_ebo{};
    int m_indexCount{};

    void m_release();
};

}  // namespace tonyMesh::app
