#pragma once

#include <core/common.hpp>
#include <core/edge.hpp>
#include <vector>

namespace tonyMesh::connectivity {

    [[nodiscard]] std::vector<Edge> edges(const FaceMatrix& faces);
    [[nodiscard]] std::vector<std::pair<Edge, std::pair<FaceIndex, FaceIndex>>> edgeTopology(const FaceMatrix& faces);
}
