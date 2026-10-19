#ifndef TONYMESH_EDGE_HPP
#define TONYMESH_EDGE_HPP

#include <definitions.hpp>

namespace tonyMesh {
    struct Edge {
        Index first, second;

        Edge(Index f, Index s) : first(std::min(f, s)), second(std::max(f, s)) {

        }
        auto operator<=>(const Edge&) const = default;

        // bool operator==(const Edge &other) const {
        //     return first==other.first && second==other.second;
        // }
        //
        // bool operator!=(const Edge &other) const {
        //     return first!=other.first || second!=other.second;
        // }
        //
        // bool operator<(const Edge& other) const noexcept {
        //     if (first == other.first)
        //         return second < other.second;
        //     return first < other.first;
        // }
    };
}

namespace std {
    template<>
    struct hash<tonyMesh::Edge> {
        std::size_t operator()(const tonyMesh::Edge& e) const noexcept {
            std::size_t h1 = std::hash<tonyMesh::Index>{}(e.first);
            std::size_t h2 = std::hash<tonyMesh::Index>{}(e.second);

            return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
        }
    };
}
#endif //TONYMESH_EDGE_HPP
