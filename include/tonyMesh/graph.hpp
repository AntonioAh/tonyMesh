#pragma once
#include <cstddef>
#include <tonyMesh/common.hpp>
#include <tuple>
#include <utility>
#include <vector>

namespace tonyMesh::graphs {

std::vector<std::pair<size_t, size_t>> mst(
    size_t numVertices, std::vector<std::tuple<size_t, size_t, Real>>& allEdges);

std::vector<size_t> connectedComponents(size_t numVertices, const std::vector<std::pair<size_t, size_t>>& edges);
std::vector<std::vector<size_t>> buildAdjList(size_t numVertices, const std::vector<std::pair<size_t, size_t>>& edges);

void dfsOrder(std::vector<size_t>& order, std::vector<size_t>& parent, size_t numVertices,
           size_t startingPoint,
           const std::vector<std::vector<size_t>>& edges);
}  // namespace tonyMesh::graphs