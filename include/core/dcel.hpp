#pragma once

#include <core/common.hpp>
#include <core/edge.hpp>
#include <unordered_map>


namespace tonyMesh {
    class Mesh;
class DCEL {
public:
    constexpr static size_t npos{static_cast<size_t>(-1)};
    constexpr static FaceIndex invalidFace = static_cast<FaceIndex>(-1);
    struct HalfEdge {
        VertexIndex target;
        FaceIndex face;
        size_t twin{npos}, next{}, prev{};

        HalfEdge(VertexIndex target, FaceIndex face) :
            target(target), face(face) {

        }
    };

    explicit DCEL(const Mesh& mesh);

    [[nodiscard]] const std::vector<size_t>& faces() const {return m_faces; }
    [[nodiscard]] const std::vector<HalfEdge>& halfEdges() const {return m_half_edges; }
    [[nodiscard]] const std::vector<size_t>& vertices() const {return m_vertices; }
    [[nodiscard]] const std::vector<size_t>& outerFaces() const {return m_outerFaces; }

    [[nodiscard]] std::vector<VertexIndex> getNeighbors(VertexIndex v) const;
    [[nodiscard]] size_t degree(VertexIndex v) const {return getNeighbors(v).size(); }
    [[nodiscard]] std::vector<FaceIndex> getIncidentFaces(VertexIndex v) const;
    [[nodiscard]] std::vector<Edge> boundaryEdges() const;
    [[nodiscard]] bool isOnBoundary(VertexIndex v) const;
    [[nodiscard]] std::vector<std::vector<VertexIndex>> vertexVertexAdjacency(bool sorted = false) const;

private:
    std::vector<size_t> m_vertices;
    std::vector<size_t> m_faces;
    std::vector<HalfEdge> m_half_edges;

    std::vector<size_t> m_outerFaces;

    typedef std::unordered_map<Edge, size_t, Edge::EdgeHash> EdgeMap;
    void m_buildInnerFaces(const Mesh& mesh, EdgeMap& edges);
    void m_buildOuterFaces(EdgeMap& edges);

};
}
