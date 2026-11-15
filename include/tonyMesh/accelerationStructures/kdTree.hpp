#pragma once
#include <cstddef>
#include <queue>
#include <tonyMesh/accelerationStructures/accelerationStructure.hpp>
#include <tonyMesh/common.hpp>
#include <utility>
#include <vector>

namespace tonyMesh::accelerationStructures {
class KdTree : public AccelerationStructure {
public:
    KdTree(const VertexMatrix& vertices, std::size_t pointsInLeaf = 10);
    std::vector<Index> knn(const Point& query, size_t k) override;
    std::vector<Index> withinRadius(const Point& query, Real radius) override;

private:
    const size_t m_pointsInLeaf{};
    std::vector<size_t> m_order;

    typedef decltype(m_order)::iterator iterator;
    typedef std::priority_queue<std::pair<DoubleReal, size_t>> Priority;
    void m_build(iterator start, iterator end, int depth);
    void m_knn(iterator start, iterator end, int depth, const Point& point, size_t k, Priority& pq);
    void m_withinRadius(iterator start, iterator end, int depth, const Point& point, const DoubleReal& limit, std::vector<Index>& result);

    static void m_addPointKnn(size_t idx, DoubleReal sqDist, Priority& pq, size_t k);

};
}