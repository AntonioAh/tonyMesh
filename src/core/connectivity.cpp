#include <core/connectivity.hpp>


namespace tonyMesh::connectivity {
    std::vector<Edge> edges(const FaceMatrix& faces) {
        std::vector<Edge> edges;
        edges.reserve(3 * faces.rows());

        for (FaceIndex i{}; i < faces.rows(); i++) {
            edges.emplace_back(faces(i, 0), faces(i, 1));
            edges.emplace_back(faces(i, 0), faces(i, 2));
            edges.emplace_back(faces(i, 1), faces(i, 2));
        }

        std::ranges::sort(edges, [](const Edge& a, const Edge& b) {
            return a < b;
        });

        auto [first, last] = std::ranges::unique(edges);
        edges.erase(first, last);

        return edges;

    }

    std::vector<std::pair<Edge, std::pair<FaceIndex, FaceIndex>>> edgeTopology(const FaceMatrix& faces) {
        std::vector<std::pair<Edge, std::pair<FaceIndex, FaceIndex>>> result;
        std::unordered_map<Edge, FaceIndex, Edge::EdgeHash> m;

        auto helper = [&](FaceIndex i, const Edge& edge) {
            auto found{m.find(edge)};
            if (found != m.end()) {
                result.push_back({edge, {i, found->second}});
                m.erase(found);
            }
            else {
                m[edge] = i;
            }
        };

        for (FaceIndex i{}; i < faces.rows(); i++) {
            for (int k{}; k < 3; k++) {
                Edge edge{faces(i, k), faces(i, (k+1)%3)};
                helper(i, edge);
            }
        }

        for (const auto& [edge, face] : m) {
            result.push_back({edge, {face, -1}});
        }
        return result;
    }
}