#pragma once

#include <core/common.hpp>
#include <core/dcel.hpp>

namespace tonyMesh {
class Mesh {
public:
    Mesh() = default;
    Mesh(const Mesh& other) = default;
    Mesh(Mesh&& other) noexcept = default;
    Mesh& operator=(const Mesh& other) = default;
    Mesh& operator=(Mesh&& other) noexcept = default;

    Mesh(int vertices, int faces);
    Mesh(const VertexMatrix& vertices, const FaceMatrix& faces, bool withDCEL=false);
    Mesh(VertexMatrix&& vertices, FaceMatrix&& faces, bool withDCEL=false);

    [[nodiscard]] Index numVertices() const { return m_vertices.rows(); }
    [[nodiscard]] Index numFaces() const {return m_faces.rows(); }

    VertexMatrix& vertices() {return m_vertices; }
    [[nodiscard]] const VertexMatrix& vertices() const {return m_vertices; }

    FaceMatrix& faces() {return m_faces; }
    [[nodiscard]] const FaceMatrix& faces() const {return m_faces; }

    // [[nodiscard]] const Vertex& getVertex(VertexIndex i) const { assert(i < m_vertices.rows()); return m_vertices.row(i); }
    // [[nodiscard]] const Face& getFace(FaceIndex i) const { assert(i < m_faces.rows()); return m_faces.row(i); }

    [[nodiscard]] std::vector<VertexIndex> getNeighbors(VertexIndex v) const;
    [[nodiscard]] size_t degree(VertexIndex v) const { return getNeighbors(v).size(); }
    [[nodiscard]] std::vector<FaceIndex> getAdjancentFaces(VertexIndex v) const;

    [[nodiscard]] std::vector<std::vector<VertexIndex>> vertexVertexAdjacency() const;
    [[nodiscard]] std::vector<std::vector<FaceIndex>> vertexFaceAdjacency() const;

    [[nodiscard]] std::vector<Edge> edges() const;
    [[nodiscard]] std::vector<Edge> boundaryEdges() const;
    [[nodiscard]] std::vector<bool> isBoundaryEdge(const std::vector<Edge>& edges) const;
    [[nodiscard]] std::pair<std::vector<Edge>, std::vector<bool>> isBoundaryEdge() const;
    [[nodiscard]] std::vector<bool> isBoundaryVertex() const;

    //TODO : TRIANGLE TRIANGLE ADJACENCY
    //TODO : TRIANGLE EDGES ADJACENCY

    [[nodiscard]] bool hasDCEL() const { return m_dcel != nullptr; }
    [[nodiscard]] std::shared_ptr<DCEL> dcel() const {return m_dcel; }
    void buildDCEL() const { m_dcel = std::make_shared<DCEL>(*this); }
protected:
    VertexMatrix m_vertices{};
    FaceMatrix m_faces{};

    mutable std::shared_ptr<DCEL> m_dcel{};
    void m_buildDCEL();

private:
    //TODO : revision needed
    [[nodiscard]] bool isOnBoundary(VertexIndex v) const;
};
}
