#include <omp.h>

#include <algorithm>
#include <edge.hpp>
#include <mesh.hpp>
#include <numbers>
#include <utility>
#include <vector>

namespace tonyMesh
{
Mesh::Mesh(const VertexMatrix& vertices, const FaceMatrix& faces)
    : m_vertices(vertices), m_faces(faces)
{
    activate();
}

Mesh::Mesh(VertexMatrix&& vertices, FaceMatrix&& faces)
    : m_vertices(std::move(vertices)), m_faces(std::move(faces))
{
    activate();
}

void Mesh::activate()
{
    m_minBounding = m_vertices.colwise().minCoeff();
    m_maxBounding = m_vertices.colwise().maxCoeff();
}

std::vector<Eigen::Index> Mesh::vertexVertexNeighbors(Eigen::Index vertexIdx) const
{
    std::vector<Index> neighbors;
    const auto nF{numFaces()};

#pragma omp parallel
    {
        std::vector<Index> temp;
#pragma omp for nowait
        for (Index f = 0; f < nF; f++) {
            if (m_faces(f, 0) == vertexIdx) {
                temp.push_back(m_faces(f, 1));
                temp.push_back(m_faces(f, 2));
            } else if (m_faces(f, 1) == vertexIdx) {
                temp.push_back(m_faces(f, 0));
                temp.push_back(m_faces(f, 2));
            } else if (m_faces(f, 2) == vertexIdx) {
                temp.push_back(m_faces(f, 0));
                temp.push_back(m_faces(f, 1));
            }
        }

#pragma omp critical
        neighbors.insert(neighbors.end(), temp.begin(), temp.end());
    }

    std::ranges::sort(neighbors);
    neighbors.erase(std::ranges::unique(neighbors).begin(), neighbors.end());
    return neighbors;
}

std::vector<std::vector<Eigen::Index>> Mesh::vertexVertexAdjacency() const
{
    const auto nV{numVertices()}, nF{numFaces()};
    std::vector<std::vector<Index>> adjacency(nV);

    for (Index f = 0; f < nF; f++) {
        adjacency[m_faces(f, 0)].push_back(m_faces(f, 1));
        adjacency[m_faces(f, 0)].push_back(m_faces(f, 2));

        adjacency[m_faces(f, 1)].push_back(m_faces(f, 0));
        adjacency[m_faces(f, 1)].push_back(m_faces(f, 2));

        adjacency[m_faces(f, 2)].push_back(m_faces(f, 0));
        adjacency[m_faces(f, 2)].push_back(m_faces(f, 1));
    }

#pragma omp parallel for
    for (Index v = 0; v < nV; v++) {
        std::ranges::sort(adjacency[v]);
        adjacency[v].erase(std::ranges::unique(adjacency[v]).begin(), adjacency[v].end());
    }

    return adjacency;
}

std::vector<FaceType> Mesh::vertexFaceNeighbors(Eigen::Index vertexIdx) const
{
    std::vector<FaceType> faces;
    const auto nF{numFaces()};

#pragma omp parallel
    {
        std::vector<FaceType> temp;

#pragma omp for nowait
        for (Index f = 0; f < nF; f++) {
            if (m_faces(f, 0) == vertexIdx || m_faces(f, 1) == vertexIdx ||
                m_faces(f, 2) == vertexIdx) {
                temp.push_back(f);
            }
        }

#pragma omp critical
        faces.insert(faces.end(), temp.begin(), temp.end());
    }
    return faces;
}

std::vector<std::vector<FaceType>> Mesh::vertexFaceAdjacency() const
{
    const auto nV{numVertices()}, nF{numFaces()};
    std::vector<std::vector<FaceType>> adjacency(nV);

    for (Index f = 0; f < nF; f++)
        for (Index i = 0; i < 3; i++)
            adjacency[m_faces(f, i)].push_back(f);

#pragma omp parallel for
    for (Index v = 0; v < nV; v++) {
        std::ranges::sort(adjacency[v]);
        adjacency[v].erase(std::ranges::unique(adjacency[v]).begin(), adjacency[v].end());
    }
    return adjacency;
}

std::vector<Edge> Mesh::edges() const
{
    std::vector<Edge> edges;

    const auto nF{numFaces()};
    for (Index f{}; f < nF; f++) {
        edges.emplace_back(m_faces(f, 0), m_faces(f, 1));
        edges.emplace_back(m_faces(f, 0), m_faces(f, 2));
        edges.emplace_back(m_faces(f, 1), m_faces(f, 2));
    }

    std::ranges::sort(edges);
    edges.erase(std::ranges::unique(edges).begin(), edges.end());
    return edges;
}

std::vector<Edge> Mesh::boundaryEdges() const
{
    std::vector<Edge> allEdges;
    const auto nF{numFaces()};
    for (Index f{}; f < nF; f++) {
        allEdges.emplace_back(m_faces(f, 0), m_faces(f, 1));
        allEdges.emplace_back(m_faces(f, 0), m_faces(f, 2));
        allEdges.emplace_back(m_faces(f, 1), m_faces(f, 2));
    }
    std::ranges::sort(allEdges);

    std::vector<Edge> edges;
    if (allEdges.size() > 1 && allEdges[0] != allEdges[1])
        edges.push_back(allEdges[1]);

    for (size_t i{1}; i < allEdges.size() - 1; i++)
        if (allEdges[i] != allEdges[i - 1] && allEdges[i] != allEdges[i + 1])
            edges.push_back(allEdges[i]);

    if (allEdges.size() >= 2 && allEdges.back() != allEdges[allEdges.size() - 2])
        edges.push_back(allEdges.back());

    return edges;
}

std::vector<Eigen::Index> Mesh::boundaryVertices() const
{
    const auto boundEdges{boundaryEdges()};
    std::vector<Index> vertices;
    for (const auto& edge : boundEdges) {
        vertices.push_back(edge.first);
        vertices.push_back(edge.second);
    }

    std::ranges::sort(vertices);
    vertices.erase(std::ranges::unique(vertices).begin(), vertices.end());
    return vertices;
}

bool Mesh::isBoundaryVertex(Eigen::Index vertexIdx) const
{
    const auto boundVertices = boundaryVertices();
    return std::ranges::binary_search(boundVertices, vertexIdx);
}

bool Mesh::isBoundaryEdge(Edge e) const
{
    const auto boundEdges = boundaryEdges();
    return std::ranges::binary_search(boundaryEdges(), e);
}

bool Mesh::isClosed() const
{
    return boundaryEdges().empty();
}

size_t Mesh::vertexValence(Eigen::Index vertexIdx) const
{
    return vertexVertexNeighbors(vertexIdx).size();
}

std::vector<size_t> Mesh::vertexValences() const
{
    const auto vv{vertexVertexAdjacency()};
    std::vector<size_t> valences(vv.size());

#pragma omp parallel for default(none) shared(vv, valences)
    for (Index i = 0; i < valences.size(); i++)
        valences[i] = vv[i].size();

    return valences;
}
}  // namespace tonyMesh
