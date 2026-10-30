#pragma once

#include <core/common.hpp>

#include <memory>

namespace tonyMesh {
    class Mesh;
    struct Edge;

class DCEL {
public:
    struct HalfEdge {
        VertexIndex target;
        FaceIndex face;
        std::shared_ptr<HalfEdge> twin{};
        std::shared_ptr<HalfEdge> next{}, prev{};

        HalfEdge(VertexIndex target, FaceIndex face) :
            target(target),
            face(face) {
        }
    };

    explicit DCEL(const Mesh& mesh);

    [[nodiscard]] size_t degree(VertexIndex v) const;
    [[nodiscard]] std::vector<VertexIndex> getNeighbors(VertexIndex v) const;

    [[nodiscard]] std::vector<FaceIndex> getIncidentFaces(VertexIndex v) const;
    [[nodiscard]] std::vector<Edge> boundaryEdges() const;

    [[nodiscard]] bool isOnBoundary(VertexIndex v) const;

    [[nodiscard]] const std::vector<std::shared_ptr<HalfEdge>>& faces() const {return m_faces; }
    [[nodiscard]] const std::vector<std::shared_ptr<HalfEdge>>& vertices() const {return m_vertices; }
    [[nodiscard]] const std::vector<std::shared_ptr<HalfEdge>>& outerFaces() const {return m_outerFaces; }



private:
    std::vector<std::shared_ptr<HalfEdge>> m_vertices;
    std::vector<std::shared_ptr<HalfEdge>> m_faces;
    std::vector<std::shared_ptr<HalfEdge>> m_outerFaces;
};
}