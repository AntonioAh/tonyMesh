#include <dataStructures.hpp>
#include <__numeric/ranges_iota.h>

using namespace tonyMesh;

UnionFind::UnionFind(size_t n) {
    _parent.resize(n);
    std::ranges::iota(_parent, 0);

    _size.resize(n, 1);
}

size_t UnionFind::find(size_t i) {
    if (_parent[i] == i)
        return i;

    _parent[i] = find(_parent[i]);
    return _parent[i];
}

bool UnionFind::unite(size_t i, size_t j) {
    size_t first{find(i)}, second{find(j)};

    if (first == second)
        return false;

    if (_size[first] > _size[second]) {
        _parent[second] = first;
        _size[first] += _size[second];
    }
    else {
        _parent[first] = second;
        _size[second] += _size[first];
    }
    return true;
}
