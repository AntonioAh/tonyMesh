#include <algorithm>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <tonyMesh/accelerationStructures/kdTree.hpp>
#include <tonyMesh/common.hpp>
#include <tonyMesh/math.hpp>
#include <utility>

namespace tonyMesh::accelerationStructures {
KdTree::KdTree(const VertexMatrix& vertices, std::size_t pointsInLeaf)
    : AccelerationStructure(vertices), m_pointsInLeaf(pointsInLeaf) {
    // NaN breaks the ordering nth_element relies on (undefined behaviour).
    if (!p_vertices.allFinite())
        throw std::invalid_argument(
            "KdTree: vertex coordinates must be finite (no NaN or infinity)");

    m_order.resize(static_cast<size_t>(p_vertices.rows()));
    std::iota(m_order.begin(), m_order.end(), size_t{0});

    m_build(m_order.begin(), m_order.end(), 0);
}

std::vector<Index> KdTree::knn(const Point& query, size_t k) {
    if (k == 0) return {};

    Priority pq;
    m_knn(m_order.begin(), m_order.end(), 0, query, k, pq);

    std::vector<Index> result;
    result.reserve(pq.size());
    while (!pq.empty()) {
        result.push_back(static_cast<Index>(pq.top().second));
        pq.pop();
    }

    std::ranges::reverse(result);
    return result;
}

std::vector<Index> KdTree::withinRadius(const Point& point, Real radius) {
    if (radius < 0) return {};

    const DoubleReal limit{static_cast<DoubleReal>(radius) * radius};
    std::vector<Index> result;
    m_withinRadius(m_order.begin(), m_order.end(), 0, point, limit, result);
    return result;
}

void KdTree::m_build(iterator start, iterator end, int depth) {
    const auto count{static_cast<size_t>(std::distance(start, end))};
    if (count <= m_pointsInLeaf) return;

    iterator mid{start + static_cast<std::ptrdiff_t>(count / 2)};

    std::nth_element(start, mid, end, [&](size_t a, size_t b) {
        return p_vertices(static_cast<Index>(a), depth) <
               p_vertices(static_cast<Index>(b), depth);
    });

    m_build(start, mid, (depth + 1) % 3);
    m_build(mid + 1, end, (depth + 1) % 3);
}

void KdTree::m_knn(iterator start, iterator end, int depth, const Point& point,
                   size_t k, Priority& pq) {
    const auto count{static_cast<size_t>(std::distance(start, end))};

    if (count <= m_pointsInLeaf) {
        for (iterator it{start}; it != end; ++it) {
            const DoubleReal sqDist{math::squaredDistance(
                point, p_vertices.row(static_cast<Index>(*it)))};
            m_addPointKnn(*it, sqDist, pq, k);
        }
        return;
    }

    iterator mid{start + static_cast<std::ptrdiff_t>(count / 2)};
    const size_t idx{*mid};
    const DoubleReal sqDist{
        math::squaredDistance(point, p_vertices.row(static_cast<Index>(idx)))};
    m_addPointKnn(idx, sqDist, pq, k);

    const DoubleReal diff{static_cast<DoubleReal>(point(depth)) -
                          p_vertices(static_cast<Index>(idx), depth)};
    const bool queryLeft{diff < 0};
    depth = (depth + 1) % 3;

    if (queryLeft)
        m_knn(start, mid, depth, point, k, pq);
    else
        m_knn(mid + 1, end, depth, point, k, pq);

    if (pq.size() < k || diff * diff < pq.top().first) {
        if (queryLeft)
            m_knn(mid + 1, end, depth, point, k, pq);
        else
            m_knn(start, mid, depth, point, k, pq);
    }
}

void KdTree::m_withinRadius(iterator start, iterator end, int depth,
                            const Point& query, const DoubleReal& limit,
                            std::vector<Index>& result) {
    const auto count{static_cast<size_t>(std::distance(start, end))};

    if (count <= m_pointsInLeaf) {
        for (iterator it{start}; it != end; ++it) {
            const auto idx{static_cast<Index>(*it)};
            if (math::squaredDistance(query, p_vertices.row(idx)) <= limit)
                result.push_back(idx);
        }
        return;
    }

    iterator mid{start + static_cast<std::ptrdiff_t>(count / 2)};
    const auto idx{static_cast<Index>(*mid)};
    if (math::squaredDistance(query, p_vertices.row(idx)) <= limit)
        result.push_back(idx);

    const DoubleReal diff{static_cast<DoubleReal>(query(depth)) -
                          p_vertices(idx, depth)};
    const bool queryLeft{diff < 0};

    depth = (depth + 1) % 3;
    if (queryLeft)
        m_withinRadius(start, mid, depth, query, limit, result);
    else
        m_withinRadius(mid + 1, end, depth, query, limit, result);

    // <= (not <): a point exactly on the sphere still counts as inside.
    if (diff * diff <= limit) {
        if (queryLeft)
            m_withinRadius(mid + 1, end, depth, query, limit, result);
        else
            m_withinRadius(start, mid, depth, query, limit, result);
    }
}

void KdTree::m_addPointKnn(size_t idx, DoubleReal sqDist, Priority& pq,
                           size_t k) {
    if (pq.size() < k) {
        pq.emplace(sqDist, idx);
        return;
    }

    pq.emplace(sqDist, idx);
    pq.pop();
}
}  // namespace tonyMesh::accelerationStructures
