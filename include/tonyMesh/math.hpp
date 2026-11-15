#pragma once

#include <tonyMesh/common.hpp>
namespace tonyMesh::math {
// Accumulated in double: casting first, so the sum is not computed in float.
inline DoubleReal squaredDistance(const Point& first, const Point& second) {
        return (first.cast<DoubleReal>() - second.cast<DoubleReal>()).squaredNorm();
    }

inline Real distance(const Point& first, const Point& second) {
        return (first - second).norm();
    }


}