#include <core/dcel.hpp>

#include <core/mesh.hpp>
#include <unordered_map>
#include <tbb/parallel_for.h>

namespace tonyMesh {
    DCEL::DCEL(const Mesh &mesh) {
        m_vertices.resize(mesh.numVertices(), static_cast<size_t>(-1));
        m_faces.reserve(mesh.numVertices());

        EdgeMap edges;
        m_buildInnerFaces(mesh, edges);
        m_buildOuterFaces(edges);
    }

    std::vector<VertexIndex> DCEL::getNeighbors(VertexIndex v) const {
        if (m_vertices[v] == npos)
            return {};

        size_t start{m_vertices[v]};
        size_t curr{start};
        std::vector<VertexIndex> result;
        do {
            result.push_back(m_half_edges[curr].target);
            curr = m_half_edges[ m_half_edges[curr].twin ].next;
        }while (curr != start);
        return result;
    }

    std::vector<FaceIndex> DCEL::getIncidentFaces(VertexIndex v) const {
        if (m_vertices[v] == npos)
            return {};

        size_t start{m_vertices[v]};
        size_t curr{start};
        std::vector<FaceIndex> result;

        do {
            if (m_half_edges[curr].face != invalidFace)
                result.push_back(m_half_edges[curr].face);
            curr = m_half_edges[m_half_edges[curr].twin].next;
        }while (curr != start);
        return result;

    }

    std::vector<Edge> DCEL::boundaryEdges() const {
        std::vector<Edge> result;

        for (size_t start : m_outerFaces) {
            auto curr{start};

            do {
                result.emplace_back(m_half_edges[curr].target, m_half_edges[m_half_edges[curr].next].target);
                curr = m_half_edges[curr].next;
            }while (curr != start);
        }

        return result;
    }

    bool DCEL::isOnBoundary(VertexIndex v) const {
        if (m_vertices[v] == npos)
            return false;

        const size_t start{m_vertices[v]};
        size_t curr{start};

        do {
            if (m_half_edges[curr].face == invalidFace)
                return true;
            curr = m_half_edges[m_half_edges[curr].twin].next;
        }while (curr != start);

        return false;
    }

    std::vector<std::vector<VertexIndex>> DCEL::vertexVertexAdjacency(bool sorted) const {
        std::vector<std::vector<VertexIndex>> result(m_vertices.size());

        tbb::parallel_for(size_t{}, m_vertices.size(), [&](size_t i) {
            result[i] = getNeighbors(i);

            if (sorted)
                std::ranges::sort(result[i]);
        });

        return result;
    }

    void DCEL::m_buildInnerFaces(const Mesh &mesh, EdgeMap &edges) {
        const auto nFaces{mesh.numFaces()};
        const auto& F{mesh.faces()};

        for (FaceIndex i{}; i < nFaces; i++) {
            const size_t base{m_half_edges.size()};
            const FaceType v[3] = {F(i, 0), F(i, 1), F(i, 2)};

            for (int k{}; k < 3; k++) {
                HalfEdge half{v[(k+1) % 3], i};
                half.prev = base + (k + 2)%3;
                half.next = base + (k + 1)%3;
                m_half_edges.push_back(half);
                m_vertices[v[k]] = base + k;
            }
            m_faces.push_back(base);

            for (int k{}; k < 3; k++) {
                const size_t idx{base + k};
                const FaceType from{v[k]}, to{v[(k+1)%3]};

                if (auto it{edges.find(Edge{to , from, false})}; it != edges.end()) {
                    m_half_edges[idx].twin = it->second;
                    m_half_edges[it->second].twin = idx;
                    edges.erase(it);
                }
                else {
                    edges.emplace(Edge{from, to, false}, idx);
                }
            }
        }
    }

    void DCEL::m_buildOuterFaces(EdgeMap& edges) {

        while (!edges.empty()) {
            const size_t firstIdx = m_half_edges.size();
            m_outerFaces.push_back(firstIdx);

            auto first = edges.begin();
            {
                HalfEdge h{first->first.first, invalidFace};
                h.twin = first->second;
                m_half_edges[first->second].twin = firstIdx;
                m_half_edges.push_back(h);
                edges.erase(first);
            }

            size_t currIdx = firstIdx;

            while (true) {
                const VertexIndex target = m_half_edges[currIdx].target;

                auto found = std::find_if(edges.begin(), edges.end(),
                    [&](const auto& p) { return p.first.second == target; });

                if (found == edges.end()) {
                    break;
                }

                const size_t newIdx = m_half_edges.size();

                HalfEdge h{found->first.first, invalidFace};
                h.twin = found->second;
                h.prev = currIdx;
                m_half_edges[found->second].twin = newIdx;
                edges.erase(found);

                m_half_edges.push_back(h);
                m_half_edges[currIdx].next = newIdx;
                currIdx = newIdx;
            }

            m_half_edges[currIdx].next = firstIdx;
            m_half_edges[firstIdx].prev = currIdx;
        }
    }
}
