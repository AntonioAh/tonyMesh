#ifndef TONYMESH_MESH_HPP
#define TONYMESH_MESH_HPP

#include <definitions.hpp>
#include <utility>
#include <array>
#include <unordered_set>
#include <vector>
#include <edge.hpp>

namespace tonyMesh {

class Mesh {
public:
    Mesh() = default;

    Mesh(const VertexMatrix& vertices, const FaceMatrix& faces);
    Mesh(VertexMatrix&& vertices, FaceMatrix&& faces);

    Mesh(const Mesh& other) = default;
    Mesh& operator=(const Mesh& other) = default;
    Mesh(Mesh&& other) noexcept = default;
    Mesh& operator=(Mesh&& other) noexcept = default;
    virtual ~Mesh() = default;

    void activate();

    [[nodiscard]] const VertexMatrix& getVertices() const noexcept { return m_vertices; }
    [[nodiscard]] VertexMatrix& getVertices() noexcept { return m_vertices; }

    [[nodiscard]] const FaceMatrix& getFaces() const noexcept { return m_faces; }
    [[nodiscard]] FaceMatrix& getFaces() noexcept { return m_faces; }

    [[nodiscard]] Eigen::Index numVertices() const noexcept { return m_vertices.rows(); }
    [[nodiscard]] Eigen::Index numFaces() const noexcept { return m_faces.rows(); }

    [[nodiscard]] Vertex getVertex(Eigen::Index idx) const noexcept { return m_vertices.row(idx); }
    [[nodiscard]] Vertex getVertex(Eigen::Index idx) noexcept { return m_vertices.row(idx); }

    [[nodiscard]] Face getFace(Eigen::Index idx) const noexcept { return m_faces.row(idx); }
    [[nodiscard]] Face getFace(Eigen::Index idx) noexcept { return m_faces.row(idx); }

    [[nodiscard]] std::array<Vertex, 2> boundingBox() const noexcept {
        return {
            m_minBounding, m_maxBounding
        };
    };

    void setVertices(const VertexMatrix& vertices) { m_vertices = vertices; }
    void setVertices(VertexMatrix&& vertices) noexcept { m_vertices = std::move(vertices); }

    void setFaces(const FaceMatrix& faces) { m_faces = faces; }
    void setFaces(FaceMatrix&& faces) noexcept { m_faces = std::move(faces); }

    [[nodiscard]] std::vector<Eigen::Index> vertexVertexNeighbors(Eigen::Index vertexIdx) const;
    [[nodiscard]] std::vector<std::vector<Eigen::Index>> vertexVertexAdjacency() const;

    [[nodiscard]] std::vector<FaceType> vertexFaceNeighbors(Eigen::Index vertexIdx) const;
    [[nodiscard]] std::vector<std::vector<FaceType>> vertexFaceAdjacency() const;

    [[nodiscard]] std::vector<FaceType> faceFaceNeighbors(Eigen::Index faceIdx) const;
    [[nodiscard]] FaceAdjacencyMatrix faceFaceAdjacency() const;

    [[nodiscard]] std::vector<Edge> edges() const;
    [[nodiscard]] std::vector<Edge> boundaryEdges() const;
    [[nodiscard]] std::vector<Eigen::Index> boundaryVertices() const;

    [[nodiscard]] bool isBoundaryVertex(Eigen::Index vertexIdx) const;
    [[nodiscard]] bool isBoundaryEdge(Edge e) const;
    [[nodiscard]] bool isClosed() const;

    [[nodiscard]] size_t vertexValence(Eigen::Index vertexIdx) const;
    [[nodiscard]] std::vector<size_t> vertexValences() const;

protected:
    VertexMatrix m_vertices;
    FaceMatrix m_faces;

    Vertex m_minBounding, m_maxBounding;
};

} // namespace tonyMesh

#endif //TONYMESH_MESH_HPP
