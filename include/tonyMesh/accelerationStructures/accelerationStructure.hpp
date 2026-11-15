#pragma once
#include <tonyMesh/common.hpp>
#include <vector>

namespace tonyMesh {
class AccelerationStructure {
public:
    virtual ~AccelerationStructure() = default;
    virtual std::vector<Index> knn(const Point& query, size_t k) = 0;
    virtual std::vector<Index> withinRadius(const Point& point, Real radius) = 0;

protected:
    AccelerationStructure(const VertexMatrix& vertices) : p_vertices(vertices) {

    }
    const VertexMatrix& p_vertices;
};
}