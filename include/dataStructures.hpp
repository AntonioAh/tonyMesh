#ifndef TONYMESH_DATASTRUCTURES_HPP
#define TONYMESH_DATASTRUCTURES_HPP

#include <vector>

namespace tonyMesh {
    class UnionFind {
    public:
        UnionFind(size_t n);
        size_t find(size_t i);
        bool unite(size_t i, size_t);

    private:
        std::vector<size_t> _parent;
        std::vector<size_t> _size;
    };
}
#endif //TONYMESH_DATASTRUCTURES_HPP
