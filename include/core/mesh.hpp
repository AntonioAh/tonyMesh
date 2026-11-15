#pragma once

#include <tonyMesh/common.hpp>
#include <tonyMesh/connectivity.hpp>

namespace tonyMesh {
class Mesh {
public:
    Mesh(VertexMatrix&& vertices, FaceMatrix&& faces) noexcept : m_vertices(std::move(vertices)), m_faces(std::move(faces)){}
    Mesh(const VertexMatrix& vertices, const FaceMatrix& faces) : m_vertices(vertices), m_faces(faces){}

    Mesh() = default;
    Mesh(const Mesh&) = default;
    Mesh(Mesh&&) noexcept = default;
    Mesh& operator=(const Mesh&) = default;
    Mesh& operator=(Mesh&&) noexcept = default;
    ~Mesh() = default;

    [[nodiscard]] Index numVertices() const noexcept {return m_vertices.cols(); }
    [[nodiscard]] Index numFaces() const noexcept {return m_faces.cols(); }

    [[nodiscard]] const VertexMatrix& vertices() const {return m_vertices; }
    [[nodiscard]] VertexMatrix& vertices() {return m_vertices; }

    [[nodiscard]] const FaceMatrix& faces() const {return m_faces; }
    [[nodiscard]] FaceMatrix& faces() {return m_faces; }

    [[nodiscard]] std::vector<Index> vertex_vertex(Index vertex) const {return connectivity::vertex_vertex(vertex, m_faces); }
    [[nodiscard]] std::vector<std::vector<Index>> vertex_vertex_lists() const {return connectivity::vertex_vertex_lists(static_cast<size_t>(m_vertices.rows()), m_faces); }

    [[nodiscard]] std::vector<Index> vertex_face(Index vertex) const {return connectivity::vertex_face(vertex, m_faces ); }
    [[nodiscard]] std::vector<std::vector<Index>> vertex_face_list() const {return connectivity::vertex_vertex_lists(static_cast<size_t>(m_vertices.rows()), m_faces); }




private:
    VertexMatrix m_vertices;
    FaceMatrix m_faces;
};
}
