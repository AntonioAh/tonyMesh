#include <algorithm>
#include <limits>
#include <stack>
#include <tonyMesh/common.hpp>
#include <tonyMesh/dataStructures.hpp>
#include <tonyMesh/graph.hpp>
#include <unordered_map>

namespace tonyMesh::graphs {

std::vector<std::pair<size_t, size_t>> mst(
    size_t numVertices, std::vector<std::tuple<size_t, size_t, Real>>& allEdges) {

    std::ranges::sort(allEdges, [](const auto& a, const auto& b) {
        return get<2>(a) < get<2>(b);
    });
    std::vector<std::pair<size_t, size_t>> result;

    dataStructures::UnionFind uf(numVertices);
    for (auto it{allEdges.begin()}; it != allEdges.end(); ++it) {
        const auto& edge{*it};

        if (uf.unite(get<0>(edge), get<1>(edge))) {
            result.emplace_back(get<0>(edge), get<1>(edge));

            if (result.size() == numVertices - 1) break;
        }
    }
    return result;
}

std::vector<size_t> connectedComponents(
    size_t numVertices, const std::vector<std::pair<size_t, size_t>>& edges) {

    dataStructures::UnionFind uf(numVertices);
    for (const auto& edge : edges) {
        uf.unite(edge.first, edge.second);
    }

    std::unordered_map<size_t, size_t> rootToLabel;
    std::vector<size_t> result(numVertices);
    for (size_t i{}; i < numVertices; i++) {
        const auto [it, inserted] = rootToLabel.try_emplace(uf.find(i), rootToLabel.size());
        result[i] = it->second;
    }
    return result;
}

std::vector<std::vector<size_t>> buildAdjList(size_t numVertices, const std::vector<std::pair<size_t, size_t>>& edges) {
    std::vector<std::vector<size_t>> result(numVertices);
    for (const auto& edge : edges) {
        result[edge.first].push_back(edge.second);
        result[edge.second].push_back(edge.first);
    }
    return result;
}

void dfsOrder(std::vector<size_t>& order, std::vector<size_t>& parent,
              size_t numVertices, size_t startingPoint,
              const std::vector<std::vector<size_t>>& edges) {

    // `parent` must have size numVertices, with unvisited entries == none.
    // Components are disjoint, so it can be shared across calls without a reset.
    constexpr size_t none{std::numeric_limits<size_t>::max()};
    order.clear();

    parent[startingPoint] = startingPoint;

    std::stack<size_t> st;
    st.push(startingPoint);

    while (!st.empty()) {
        const auto node{st.top()};
        st.pop();
        order.push_back(node);

        for (size_t neigh : edges[node]) {
            if (parent[neigh] == none) {
                parent[neigh] = node;
                st.push(neigh);
            }
        }
    }
}
}  // namespace tonyMesh::graphs
