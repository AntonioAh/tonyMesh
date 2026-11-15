#pragma once
#include <tonyMesh/common.hpp>

namespace tonyMesh {
    struct Edge {
        FaceType first, second;

        Edge(FaceType f, FaceType s) {
            if (f < s) {
                first = f;
                second = s;
            }
            else {
                first = s;
                second = f;
            }
        }

        auto operator<=>(const Edge &) const = default;
    };
}