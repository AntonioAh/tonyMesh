#pragma once

#include <cassert>
#include <core/common.hpp>

namespace tonyMesh {

class Mesh {
public:
    Mesh() = default;
    Mesh(const Mesh& other) = default;
    Mesh(Mesh&& other) noexcept = default;
    Mesh& operator=(const Mesh& other) = default;
    Mesh& operator=(Mesh&& other) noexcept = default;

    Mesh(int vertices, int faces);
    Mesh(const VertexMatrix& vertices, const FaceMatrix& faces);
    Mesh(VertexMatrix&& vertices, FaceMatrix&& faces);

    [[nodiscard]] Index numVertices() const { return m_vertices.rows(); }
    [[nodiscard]] Index numFaces() const {return m_faces.rows(); }

    VertexMatrix& vertices() {return m_vertices; }
    [[nodiscard]] const VertexMatrix& vertices() const {return m_vertices; }

    FaceMatrix& faces() {return m_faces; }
    [[nodiscard]] const FaceMatrix& faces() const {return m_faces; }

    // [[nodiscard]] const Vertex& getVertex(VertexIndex i) const { assert(i < m_vertices.rows()); return m_vertices.row(i); }
    // [[nodiscard]] const Face& getFace(FaceIndex i) const { assert(i < m_faces.rows()); return m_faces.row(i); }

    [[nodiscard]] std::vector<VertexIndex> getNeighbors(VertexIndex v) const;
    [[nodiscard]] std::vector<FaceIndex> getAdjancentFaces(VertexIndex v) const;

    [[nodiscard]] std::vector<std::vector<VertexIndex>> vertexVertexAdjacency() const;
    [[nodiscard]] std::vector<std::vector<FaceIndex>> vertexFaceAdjacency() const;


protected:
    VertexMatrix m_vertices;
    FaceMatrix m_faces;
};
}