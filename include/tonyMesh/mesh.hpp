#pragma once

#include <tonyMesh/common.hpp>
#include <tonyMesh/connectivity.hpp>

#include "normals.hpp"

namespace tonyMesh {
class Mesh {
public:
    Mesh(VertexMatrix&& vertices, FaceMatrix&& faces) noexcept
        : m_vertices(std::move(vertices)), m_faces(std::move(faces)) {
        m_build();
    }
    Mesh(const VertexMatrix& vertices, const FaceMatrix& faces)
        : m_vertices(vertices), m_faces(faces) {
        m_build();
    }

    Mesh() = default;
    Mesh(const Mesh&) = default;
    Mesh(Mesh&&) noexcept = default;
    Mesh& operator=(const Mesh&) = default;
    Mesh& operator=(Mesh&&) noexcept = default;
    ~Mesh() = default;

    [[nodiscard]] Index numVertices() const noexcept {
        return m_vertices.rows();
    }
    [[nodiscard]] Index numFaces() const noexcept { return m_faces.rows(); }

    [[nodiscard]] const VertexMatrix& vertices() const { return m_vertices; }
    [[nodiscard]] VertexMatrix& vertices() { return m_vertices; }

    [[nodiscard]] const FaceMatrix& faces() const { return m_faces; }
    [[nodiscard]] FaceMatrix& faces() { return m_faces; }

    [[nodiscard]] std::vector<Index> vertex_vertex(Index vertex) const {
        return connectivity::vertex_vertex(vertex, m_faces);
    }
    [[nodiscard]] std::vector<std::vector<Index>> vertex_vertex_lists() const {
        return connectivity::vertex_vertex_lists(
            static_cast<size_t>(m_vertices.rows()), m_faces);
    }
    [[nodiscard]] int degree(Index vertex) const {
        return connectivity::degree(vertex, m_faces);
    }
    [[nodiscard]] std::vector<int> degree() const {
        return connectivity::degrees(static_cast<size_t>(m_vertices.rows()),
                                     m_faces);
    }

    [[nodiscard]] std::vector<Index> vertex_face(Index vertex) const {
        return connectivity::vertex_face(vertex, m_faces);
    }
    [[nodiscard]] std::vector<std::vector<Index>> vertex_face_list() const {
        return connectivity::vertex_faces_lists(m_vertices.rows(), m_faces);
    }

    [[nodiscard]] std::vector<Edge> edges() const {
        return connectivity::edges(m_faces);
    }

    [[nodiscard]] std::vector<bool> isEdgeManifold() const {
        return connectivity::isEdgeManifold(m_faces);
    }
    [[nodiscard]] std::vector<bool> isVertexManifold() const {
        return connectivity::isVertexManifold(
            static_cast<size_t>(m_vertices.rows()), faces());
    }
    [[nodiscard]] bool isManifold() const {
        return connectivity::isManifold(static_cast<size_t>(m_vertices.rows()),
                                        faces());
    }

    [[nodiscard]] NormalMatrix faceNormals() const {
        return normals::faceNormals(m_vertices, m_faces);
    }
    [[nodiscard]] const NormalMatrix& normals() const { return m_normals; }
    [[nodiscard]] NormalMatrix& normals() { return m_normals; }
    [[nodiscard]] const NormalMatrix& computeNormals(
        normals::Normalization type) {
        m_normals = normals::vertexNormals(m_vertices, m_faces, type);
        return m_normals;
    }

private:
    VertexMatrix m_vertices;
    FaceMatrix m_faces;
    NormalMatrix m_normals;

    void m_build() { m_normals = normals::vertexNormals(m_vertices, m_faces); }
};
}  // namespace tonyMesh
