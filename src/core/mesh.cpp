#include <core/mesh.hpp>
#include <ranges>
#include <algorithm>
#include <tbb/parallel_for.h>

namespace tonyMesh {
    Mesh::Mesh(int vertices, int faces) {
        m_vertices.resize(vertices, 3);
        m_faces.resize(faces, 3);
    }

    Mesh::Mesh(const VertexMatrix &vertices, const FaceMatrix &faces) :
    m_vertices(vertices), m_faces(faces)
    {

    }

    Mesh::Mesh(VertexMatrix &&vertices, FaceMatrix &&faces) :
    m_vertices(std::move(vertices)), m_faces(std::move(faces))
    {

    }

    std::vector<VertexIndex> Mesh::getNeighbors(VertexIndex v) const {
        std::vector<VertexIndex> neighs;
        for (FaceIndex i{}; i < m_faces.rows(); i++) {
            if (m_faces(i, 0) == v) {
                neighs.push_back(m_faces(i, 1));
                neighs.push_back(m_faces(i, 2));
            }
            else if (m_faces(i, 1) == v) {
                neighs.push_back(m_faces(i, 0));
                neighs.push_back(m_faces(i, 2));
            }
            else if (m_faces(i, 2) == v) {
                neighs.push_back(m_faces(i, 0));
                neighs.push_back(m_faces(i, 1));
            }
        }
        std::ranges::sort(neighs);
        neighs.erase(std::ranges::unique(neighs).begin(), neighs.end());
        return neighs;
    }

    std::vector<FaceIndex> Mesh::getAdjancentFaces(VertexIndex v) const {
        std::vector<FaceIndex> faces;
        for (FaceIndex i{}; i < m_faces.rows(); i++) {
            if (m_faces(i, 0) == v || m_faces(i, 1) == v || m_faces(i, 2) == v)
                faces.push_back(i);
        }
        return faces;
    }

    std::vector<std::vector<VertexIndex>> Mesh::vertexVertexAdjacency() const {
        std::vector<std::vector<VertexIndex>> adjacency(m_vertices.rows());

        for (FaceIndex i{}; i < m_faces.rows(); i++) {
            adjacency[m_faces(i, 0)].push_back(m_faces(i, 1));
            adjacency[m_faces(i, 0)].push_back(m_faces(i, 2));

            adjacency[m_faces(i, 1)].push_back(m_faces(i, 0));
            adjacency[m_faces(i, 1)].push_back(m_faces(i,2));

            adjacency[m_faces(i, 2)].push_back(m_faces(i, 0));
            adjacency[m_faces(i, 2)].push_back(m_faces(i, 1));
        }

        tbb::parallel_for(static_cast<std::size_t>(0), adjacency.size(), [&](VertexIndex i) {
            std::ranges::sort(adjacency[i]);
            adjacency[i].erase(std::ranges::unique(adjacency[i].begin(), adjacency[i].end()).begin(), adjacency[i].end());
        });

        return adjacency;
    }

    std::vector<std::vector<FaceIndex>> Mesh::vertexFaceAdjacency() const {
        std::vector<std::vector<FaceIndex>> faces(m_vertices.rows());

        for (FaceIndex i{}; i < m_faces.rows(); i++) {
            faces[m_faces(i, 0)].push_back(i);
            faces[m_faces(i, 1)].push_back(i);
            faces[m_faces(i, 2)].push_back(i);
        }
        return faces;
    }
}
