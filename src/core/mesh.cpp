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

    std::vector<Edge> Mesh::edges() const {
        std::vector<Edge> edges;
        edges.reserve(3 * m_faces.rows());

        for (FaceIndex i{}; i < m_faces.rows(); i++) {
            edges.emplace_back(m_faces(i, 0), m_faces(i, 1));
            edges.emplace_back(m_faces(i, 0), m_faces(i, 2));
            edges.emplace_back(m_faces(i, 1), m_faces(i, 2));
        }

        std::ranges::sort(edges, [](const Edge& a, const Edge& b) {
            return a < b;
        });

        auto [first, last] = std::ranges::unique(edges);
        edges.erase(first, last);

        return edges;
    }

    std::vector<Edge> Mesh::boundaryEdges() const {
        std::vector<Edge> edges;
        edges.reserve(3 * m_faces.rows());

        for (FaceIndex i{}; i < m_faces.rows(); i++) {
            edges.emplace_back(m_faces(i, 0), m_faces(i, 1));
            edges.emplace_back(m_faces(i, 0), m_faces(i, 2));
            edges.emplace_back(m_faces(i, 1), m_faces(i, 2));
        }

        std::ranges::sort(edges, [](const Edge& a, const Edge& b) {
            return a < b;
        });

        std::vector<Edge> result;
        size_t counter{};
        while (counter < edges.size()) {
            if (counter == edges.size()-1 || edges[counter] != edges[counter+1]) {
                result.push_back(edges[counter]);
                counter++;
            }
            else
                counter += 2;
        }
        return result;
    }

    std::vector<bool> Mesh::isBoundaryEdge(const std::vector<Edge> &edges) const {
        std::vector<bool> count(edges.size(), false);

        for (FaceIndex i{}; i < m_faces.rows(); i++) {
            Edge first{m_faces(i, 0), m_faces(i, 1)};
            Edge second{m_faces(i, 0), m_faces(i, 2)};
            Edge third{m_faces(i, 1), m_faces(i, 2)};

            auto f{std::lower_bound(edges.begin(), edges.end(), first) - edges.begin()};
            auto s{std::lower_bound(edges.begin(), edges.end(), second) - edges.begin()};
            auto t{std::lower_bound(edges.begin(), edges.end(), third) - edges.begin()};

            count[f] = !count[f];
            count[s] = !count[s];
            count[t] = !count[t];
        }

        return count;

    }

    std::pair<std::vector<Edge>, std::vector<bool>> Mesh::isBoundaryEdge() const {
        std::vector<Edge> edges;

        edges.reserve(3 * m_faces.rows());
        for (FaceIndex i{}; i < m_faces.rows(); i++) {
            edges.emplace_back(m_faces(i, 0), m_faces(i, 1));
            edges.emplace_back(m_faces(i, 0), m_faces(i, 2));
            edges.emplace_back(m_faces(i, 1), m_faces(i, 2));
        }

        std::ranges::sort(edges, [](const Edge& a, const Edge& b) {
            return a < b;
        });

        size_t i{};
        std::vector<Edge> unique;
        std::vector<bool> isBound;

        while (i < edges.size()) {
            unique.push_back(edges[i]);
            bool jump{i == edges.size()-1 || edges[i] == edges[i+1]};
            isBound.push_back(!jump);

            i = i + (jump ? 2 : 1);
        }


        return {unique, isBound};
    }
}
