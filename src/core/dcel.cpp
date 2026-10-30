#include <core/dcel.hpp>
#include <core/mesh.hpp>

#include <unordered_map>

namespace tonyMesh {
    DCEL::DCEL(const Mesh &mesh) {
        m_vertices.resize(mesh.numVertices());
        m_faces.resize(mesh.numFaces());

        std::unordered_map<Edge, std::shared_ptr<HalfEdge>, Edge::EdgeHash> edgeMap;
        for (FaceIndex i{}; i < mesh.numFaces(); i++) {
                auto first = std::make_shared<HalfEdge>(mesh.faces()(i, 1), i); //0->1
                auto second = std::make_shared<HalfEdge>(mesh.faces()(i, 2), i); //1->2
                auto third = std::make_shared<HalfEdge>(mesh.faces()(i, 0), i); //2->0

                m_faces[i] = first;
                m_vertices[mesh.faces()(i, 0)] = first;
                m_vertices[mesh.faces()(i, 1)] = second;
                m_vertices[mesh.faces()(i, 2)] = third;


            first->prev = third;
            first->next = second;

            second->prev = first;
            second->next = third;

            third->prev = second;
            third->next = first;

            Edge e0{mesh.faces()(i, 0), mesh.faces()(i, 1), false};
            Edge e1{mesh.faces()(i, 1), mesh.faces()(i, 2), false};
            Edge e2{mesh.faces()(i, 2), mesh.faces()(i, 0), false};

            if (const auto it{edgeMap.find(Edge{e0.second, e0.first, false})}; it != edgeMap.end()) {
                first->twin = it->second;
                it->second->twin = first;
                edgeMap.erase(it);
            }
            else {
                edgeMap.emplace(e0, first);
            }

            if (const auto it{edgeMap.find(Edge{e1.second, e1.first, false})}; it != edgeMap.end()) {
                second->twin = it->second;
                it->second->twin = second;
                edgeMap.erase(it);
            }
            else {
                edgeMap.emplace(e1, second);
            }

            if (const auto it{edgeMap.find(Edge{e2.second, e2.first, false})}; it != edgeMap.end()) {
                third->twin = it->second;
                it->second->twin = third;
                edgeMap.erase(it);
            }
            else {
                edgeMap.emplace(e2, third);
            }
        }

        while (!edgeMap.empty()) {
            auto first{edgeMap.begin()};
            auto firstCreated = std::make_shared<HalfEdge>(first->first.first, -1);
            firstCreated->twin = first->second;
            first->second->twin = firstCreated;
            edgeMap.erase(first);

            m_outerFaces.push_back(firstCreated);
            auto found{
                std::ranges::find_if(edgeMap, [&](const auto& p) {
                    return p.first.second == firstCreated->target;
                })
            };

            auto curr{m_outerFaces.back()};
            while (found != edgeMap.end()) {
                auto created{std::make_shared<HalfEdge>(found->first.first, -1)};
                created->twin = found->second;
                found->second->twin = created;

                created->prev = curr;
                curr->next = created;

                curr = created;
                edgeMap.erase(found);

                found = std::ranges::find_if(edgeMap, [&](const auto& p) {
                    return p.first.second == curr->target;
                });
            }
            curr->next = firstCreated;
            firstCreated->prev = curr;
        }
    }

    size_t DCEL::degree(VertexIndex v) const {
        return getNeighbors(v).size();
    }

    std::vector<VertexIndex> DCEL::getNeighbors(VertexIndex v) const {
        if (v >= static_cast<VertexIndex>(m_vertices.size()) || !m_vertices[v])
            return {};

        std::vector<VertexIndex> result;
        auto start = m_vertices[v];
        auto curr = start;

        do {
            result.push_back(curr->target);
            curr = curr->prev->twin;
            // if (!curr) break;
        } while (curr != start);

        return result;
    }

    std::vector<FaceIndex> DCEL::getIncidentFaces(VertexIndex v) const{
        if (v >= static_cast<VertexIndex>(m_vertices.size()) || !m_vertices[v])
            return {};

        std::vector<FaceIndex> result;
        auto start{m_vertices[v]};
        auto curr{m_vertices[v]};

        do {
            if (curr->face != -1)
                result.push_back(curr->face);

            curr = curr->prev->twin;
        } while (curr != start);

        return result;
    }

    std::vector<Edge> DCEL::boundaryEdges() const {
        std::vector<Edge> result;

        for (auto start : m_outerFaces) {
            VertexIndex end{start->prev->target};
            auto curr{start};

            do {
                result.emplace_back(end, curr->target);
                end = curr->target;
                curr = curr->next;
            } while (curr != start);
        }

        return result;
    }

    bool DCEL::isOnBoundary(VertexIndex v) const {
        if (v >= static_cast<VertexIndex>(m_vertices.size()) || !m_vertices[v])
            return {};

        std::vector<FaceIndex> result;
        auto start{m_vertices[v]};
        auto curr{m_vertices[v]};

        do {
            if (curr->face == -1)
                return true;

            curr = curr->prev->twin;
        } while (curr != start);
        return false;
    }
}
