#pragma once
#include <vector>

namespace tonyMesh::dataStructures {
class UnionFind {
public:
    explicit UnionFind(size_t n) {
        m_sizes.resize(n, 1);
        m_parents.resize(n);
        for (size_t i{}; i < n; i++) {
            m_parents[i] = i;
        }
    }

    size_t find(size_t node) {
        while (m_parents[node] != node) {
            m_parents[node] = m_parents[m_parents[node]];
            node = m_parents[node];
        }
        return m_parents[node];
    }

    bool unite(size_t first, size_t second) {
        const auto a{find(first)}, b{find(second)};
        if (a == b) {
            return false;
        }

        if (m_sizes[a] > m_sizes[b]) {
            m_sizes[a] += m_sizes[b];
            m_parents[b] = a;
        } else {
            m_sizes[b] += m_sizes[a];
            m_parents[a] = b;
        }
        return true;
    }

private:
    std::vector<size_t> m_parents;
    std::vector<size_t> m_sizes;
};
}  // namespace tonyMesh::dataStructures