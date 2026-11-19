#include <glad/glad.h>

#include <Eigen/Core>
#include <stdexcept>
#include <tonyMesh/app/gpuMesh.hpp>
#include <tonyMesh/mesh.hpp>
#include <utility>

namespace tonyMesh::app {

GpuMesh::GpuMesh(const Mesh& mesh) {
    if (mesh.normals().rows() != mesh.numVertices())
        throw std::runtime_error("GpuMesh: mesh has no vertex normals");

    using VertexData = Eigen::Matrix<float, Eigen::Dynamic, 6, Eigen::RowMajor>;
    using IndexData = Eigen::Matrix<GLuint, Eigen::Dynamic, 3, Eigen::RowMajor>;

    // Column-major Eigen -> row-major, interleaved: x y z nx ny nz per row.
    VertexData vertexData(mesh.numVertices(), 6);
    vertexData.leftCols<3>() = mesh.vertices();
    vertexData.rightCols<3>() = mesh.normals();

    const IndexData indices = mesh.faces().cast<GLuint>();
    m_indexCount = static_cast<int>(indices.size());

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);  // everything below is recorded into this VAO

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(
                     static_cast<std::size_t>(vertexData.size()) * sizeof(float)),
                 vertexData.data(), GL_STATIC_DRAW);

    // The EBO binding is stored in the VAO, so bind it while the VAO is bound.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(
                     static_cast<std::size_t>(indices.size()) * sizeof(GLuint)),
                 indices.data(), GL_STATIC_DRAW);

    constexpr GLsizei stride = 6 * sizeof(float);
    // location 0 = position (layout(location = 0) in vec3 aPosition)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);
    // location 1 = normal, after the 3 position floats
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind the VAO first: unbinding the EBO before would remove it from the VAO.
    glBindVertexArray(0);
}

void GpuMesh::draw() const {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void GpuMesh::m_release() {
    if (m_vao != 0) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo != 0) glDeleteBuffers(1, &m_vbo);
    if (m_ebo != 0) glDeleteBuffers(1, &m_ebo);
    m_vao = m_vbo = m_ebo = 0;
}

GpuMesh::~GpuMesh() { m_release(); }

GpuMesh::GpuMesh(GpuMesh&& other) noexcept
    : m_vao(std::exchange(other.m_vao, 0)),
      m_vbo(std::exchange(other.m_vbo, 0)),
      m_ebo(std::exchange(other.m_ebo, 0)),
      m_indexCount(std::exchange(other.m_indexCount, 0)) {}

GpuMesh& GpuMesh::operator=(GpuMesh&& other) noexcept {
    if (this != &other) {
        m_release();
        m_vao = std::exchange(other.m_vao, 0);
        m_vbo = std::exchange(other.m_vbo, 0);
        m_ebo = std::exchange(other.m_ebo, 0);
        m_indexCount = std::exchange(other.m_indexCount, 0);
    }
    return *this;
}

}  // namespace tonyMesh::app
